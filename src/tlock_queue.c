#include <stdlib.h>
#include <string.h>

#ifdef __STDC_NO_THREADS__
	#pragma message ( "No C11 thread support" )
#else

#include "tlock_queue.h"

/* Helper function to allocate and initialize a queue node */
#ifdef __GNUC__
__attribute__ ((malloc))
#endif
inline static _tlock_node_t* _tlock_node_init(void* value) {
	_tlock_node_t* node;

	node = calloc(1, sizeof(*node));
	if (node)
		node->value = value;
	return node;
}

/* Helper function to free a queue node */
inline static void _tlock_node_free(_tlock_node_t* node) {
	free(node);
}

/* Initializes the given queue */
int tlock_init(tlock_queue_t* queue) {
	_tlock_node_t* dummy;

	/* Initialize mutexes */
	if (mtx_init(&queue->first_mutex, mtx_plain) != thrd_success)
		goto err0;
	if (mtx_init(&queue->last_mutex, mtx_plain) != thrd_success)
		goto err1;

	/* Allocate dummy node */
	dummy = _tlock_node_init(NULL);
	if (!dummy)
		goto err2;

	/* Initialize ends of queue */
	queue->first = queue->last = dummy;
	return TLOCK_OK;

err2:
	mtx_destroy(&queue->last_mutex);
err1:
	mtx_destroy(&queue->first_mutex);
err0:
	return TLOCK_ERROR;
}

/* Frees queue resources */
void tlock_free(tlock_queue_t* queue) {
	_tlock_node_t* node;
	_tlock_node_t* prev;

	if (!queue)
		return;

	node = queue->first;
	while (node) {
		prev = node;
		node = prev->next;
		_tlock_node_free(prev);
	}

	mtx_destroy(&queue->first_mutex);
	mtx_destroy(&queue->last_mutex);
}

/* Push at the end of the queue */
#ifdef __GNUC__
__attribute__ ((warn_unused_result))
#endif
int tlock_push(tlock_queue_t* restrict queue, void* restrict new_element) {
	_tlock_node_t* node;

	/* Prepare new node */
	node = _tlock_node_init(new_element);
	if (!node)
		return TLOCK_ERROR;

	/* Add to queue with lock */
	mtx_lock(&queue->last_mutex);
	queue->last->next = node;
	queue->last = node;
	mtx_unlock(&queue->last_mutex);

	return TLOCK_OK;
}

/* Pop from beginning of queue */
void* tlock_pop(tlock_queue_t* queue) {
	/* Node to be removed */
	_tlock_node_t* node;
	/* New queue head */
	_tlock_node_t* new_header;
	/* Data to be retrieved */
	void* return_value;

	mtx_lock(&queue->first_mutex);
	
	node = queue->first;
	new_header = queue->first->next;

	/* Queue is empty */
	if (!new_header) {
		mtx_unlock(&queue->first_mutex);
		return NULL;
	}

	/* Queue not empty: retrieve data and rewire */
	return_value = new_header->value;
	queue->first = new_header;

	mtx_unlock(&queue->first_mutex);

	/* Free removed node and return */
	_tlock_node_free(node);
	return return_value;
}

/*
 * Retrieves the minimum number of elements in the queue at the time of function call. The number
 * can be bigger if threads are pushing to the queue concurrently.
 */
size_t tlock_min_size(tlock_queue_t* queue) {
	register size_t counter = 0;
	_tlock_node_t* node;

	mtx_lock(&queue->first_mutex);

	/* Get first element if queue is not empty */
	node = queue->first->next;
	if (node)
		++counter;

	/* Count the rest of elements */
	while (node && node->next) {
		++counter;
		node = node->next;
	}

	mtx_unlock(&queue->first_mutex);

	return counter;
}

#endif
