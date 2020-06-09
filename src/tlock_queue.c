#include <stdlib.h>
#include <string.h>

#ifndef __STDC_NO_THREADS__

#include "tlock_queue.h"

/* Allocates and initializes queue node */
inline static _tlock_node_t* _tlock_node_init(void* value) {
	_tlock_node_t* node;

	if ( (node = malloc(sizeof(_tlock_node_t))) == NULL )
		return NULL;

	node->value = value;
	node->next = NULL;

	return node;
}

/* Frees queue node */
inline static void _tlock_node_free(_tlock_node_t* node) {
	free(node);
}

/* Initializes queue */
tlock_queue_t* tlock_init() {
	tlock_queue_t* queue;
	_tlock_node_t* free_node;

	/* Allocate queue */
	if ( (queue = malloc(sizeof(tlock_queue_t))) == NULL )
		return NULL;

	/* Allocate mutexes */
	if ( (queue->first_mutex = malloc(sizeof(mtx_t))) == NULL ) {
		free(queue);
		return NULL;
	}
	if ( (queue->last_mutex = malloc(sizeof(mtx_t))) == NULL ) {
		free(queue->first_mutex);
		free(queue);
		return NULL;		
	}

	/* Initialize mutexes */
	if (mtx_init(queue->first_mutex, mtx_plain) != thrd_success || mtx_init(queue->last_mutex, mtx_plain) != thrd_success) {
		tlock_free(queue);
		return NULL;
	}

	/* Allocate dummy node */
	if ( (free_node = _tlock_node_init(NULL)) == NULL ) {
		tlock_free(queue);
		return NULL;
	}

	/* Initialize ends of queue */
	queue->first = queue->last = free_node;

	return queue;
}

/* Frees queue resources. Assumes the queue is depleted */
void tlock_free(tlock_queue_t* queue) {

	/* Free the dummy node */
	if (queue->first != NULL) {
		free(queue->first);
	}

	/* Destroy and free mutexes */
	if (queue->first_mutex != NULL ){
		mtx_destroy(queue->first_mutex);
		free(queue->first_mutex);
	}
	if (queue->last_mutex != NULL) {
		mtx_destroy(queue->last_mutex);
		free(queue->last_mutex);
	}

	free(queue);
}

/* Push at the end of the queue */
int tlock_push(tlock_queue_t* queue, void* new_element) {
	_tlock_node_t* node;

	/* Prepare new node */
	if ( (node = _tlock_node_init(new_element)) == NULL )
		return TLOCK_ERROR;

	/* Add to queue with lock */
	mtx_lock(queue->last_mutex);
	queue->last->next = node;
	queue->last = node;
	mtx_unlock(queue->last_mutex);

	return TLOCK_OK;
}

/* Pop from beginning of queue */
void* tlock_pop(tlock_queue_t* queue) {
	_tlock_node_t* node;		/* Node to be removed */
	_tlock_node_t* new_header;	/* Node that will be come the first in the queue */
	void* return_value;		/* Data to be retrieved */

	mtx_lock(queue->first_mutex);
	
	node = queue->first;
	new_header = queue->first->next;

	/* Queue empty */
	if (new_header == NULL) {
		mtx_unlock(queue->first_mutex);
		return NULL;
	}

	/* Queue not empty: retrieve data and rewire */
	return_value = new_header->value;
	queue->first = new_header;

	mtx_unlock(queue->first_mutex);

	/* Free removed node and return */
	_tlock_node_free(node);
	return return_value;
}

/*
 * Copies the next element in the queue to 'location' and does not remove
 * it from the queue. The size of the element in the queue must be given.
 */
int tlock_see(tlock_queue_t* queue, void* location, size_t size) {
	_tlock_node_t* header;

	/* Lock queue and retrieve first node containing data */
	mtx_lock(queue->first_mutex);
	header = queue->first->next;

	/* Queue empty */
	if (header == NULL) {
		mtx_unlock(queue->first_mutex);
		return TLOCK_EMPTY;
	}

	/* Queue not empty: retrieve value and unlock queue */
	memcpy(location, header->value, size);
	mtx_unlock(queue->first_mutex);

	return TLOCK_OK;
}

/*
 * Retrieves the minimum number of elements in the queue
 * at the time of function call. The number can be bigger
 * if threads are pushing to the queue concurrently.
 */
size_t tlock_min_size(tlock_queue_t* queue){

	long long int counter = 0;
	_tlock_node_t* node;

	mtx_lock(queue->first_mutex);

	/* Get first element if queue is not empty */
	if ( (node = queue->first->next) != NULL ) {
		++counter;
	}

	/* Count the rest of elements */
	while (node!=NULL && node->next!=NULL){
		++counter;
		node = node->next;
	}

	mtx_unlock(queue->first_mutex);

	return counter;
}

#else
	#pragma message ( "No C11 thread support" )
#endif
