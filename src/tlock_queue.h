#ifndef _TLOCK_QUEUE_H
#define _TLOCK_QUEUE_H

#include <threads.h>

#define PUSH_FAILURE -2
#define PUSH_OK 0

/*
 * Queue node. For internal use only, the user should not interact with this struct.
 */
typedef struct {
	void* value;
	void* next;
} _tlock_node_t;

/*
 * Two lock queue data structure. The user should not interact directly with it, but
 * rather through the available functions.
 */
typedef struct {
	_tlock_node_t* first;
	_tlock_node_t* last;
	mtx_t* first_mutex;
	mtx_t* last_mutex;
} tlock_queue_t;

/*
 * Initialization and free functions. The tlock_free() function assumes the queue is depleted,
 * and will not manage any elements inside it that are still allocated.
 */
tlock_queue_t* tlock_init();
void tlock_free(tlock_queue_t* queue);

/* 
 * Add and remove elements from queue. Popping a value effectively removes it from the queue,
 * ensuring no other thread can pop that same value.
 */
int tlock_push(tlock_queue_t* queue, void* new_element);
void* tlock_pop(tlock_queue_t* queue);

/* 
 * Get number of elements in queue. The value returned is not precise, as this function only locks
 * the beginning of the queue, so the return value only garantees the minimum size at the time of the
 * function call, but may be bigger if other threads were adding elements concurrently. Consequently,
 * this function will block pop calls from other threads, but not push calls.
 */
long long int tlock_length(tlock_queue_t* queue);

#endif
