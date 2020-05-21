#ifndef _TLOCK_QUEUE_H
#define _TLOCK_QUEUE_H

#include <threads.h>

#define TLOCK_OK 1
#define TLOCK_EMPTY 0
#define TLOCK_ERROR -1

/*
 * Queue node. For internal use only, the user should not interact with this struct.
 */
typedef struct {
	void* value;
	void* next;
} _tlock_node_t;

/*
 * Two lock queue data structure. The user should not interact directly with it, but rather through
 * the available functions.
 */
typedef struct {
	_tlock_node_t* first;
	_tlock_node_t* last;
	mtx_t* first_mutex;
	mtx_t* last_mutex;
} tlock_queue_t;

/*
 * Returns a pointer to an allocated struct for the synchronized queue or NULL on failure.
 */
tlock_queue_t* tlock_init();

/*
 * Frees the queue struct. It assumes that the queue is depleted, and it will not manage allocated
 * elements inside of it.
 */
void tlock_free(tlock_queue_t* queue);

/*
 * Add element to queue. The client is responsible for freeing elementsput into the queue
 * afterwards.
 * Returns TLOCK_OK on success or TLOCK_ERROR on failure.
 */
int tlock_push(tlock_queue_t* queue, void* new_element);

/*
 * Retrieve element and remove it from the queue.
 * Returns a pointer to the element previously pushed in or NULL of the queue is emtpy.
 */
void* tlock_pop(tlock_queue_t* queue);

/*
 * Copies the next element in the queue to the specified location, and DOES NOT remove it from the
 * queue.
 * Returns TLOCK_OK on success or TLOCK_EMPTY if the queue is emtpy. 
 */
int tlock_see(tlock_queue_t* queue, void* location, size_t size);

/* 
 * Get number of elements in queue. The value returned is not precise, as this function only locks
 * the beginning of the queue, so the return value only garantees the MINIMUM size at the time of
 * the function call, but may be bigger if other threads were adding elements concurrently.
 * Consequently, this function will block pop calls from other threads, but not push calls.
 * Returns nº of elements or TLOCK_EMPTY
 */
size_t tlock_min_size(tlock_queue_t* queue);

#endif
