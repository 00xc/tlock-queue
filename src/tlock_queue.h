#ifndef _DOUBLE_BLOCK_H
#define _DOUBLE_BLOCK_H

#include <threads.h>

#define PUSH_FAILURE -2
#define PUSH_OK 0

typedef struct {
	void* value;
	void* next;
} _tlock_node_t;

typedef struct {
	_tlock_node_t* first;
	_tlock_node_t* last;
	mtx_t* first_mutex;
	mtx_t* last_mutex;
} tlock_queue_t;

/* Initialization and free functions */
tlock_queue_t* tlock_init();
void tlock_free(tlock_queue_t* queue);

/* Add and remove elements from queue */
int tlock_push(tlock_queue_t* queue, void* new_element);
void* tlock_pop(tlock_queue_t* queue) ;

#endif