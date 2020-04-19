#include <stdio.h>

#ifndef __STDC_NO_THREADS__

#include <stdlib.h>
#include "tlock_queue.h"

#define NUM_PUSH_THREADS 4
#define NUM_POP_THREADS 6
#define NUMS 1000000

/* This thread writes integers into the queue */
int push_thread(void* queue_ptr) {
	tlock_queue_t* queue = (tlock_queue_t*) queue_ptr;
	int* pushed_value;
	int i;

	/* Push ints into queue */
	for (i=0; i<NUMS; ++i) {
		pushed_value = malloc(sizeof(int));
		*pushed_value = i;
		if (tlock_push(queue, pushed_value) != PUSH_OK ) {
			printf("Error pushing element %i\n", i);
		}
	}

	thrd_exit(0);
}

/* This thread reads ints from the queue and frees them */
int pop_thread(void* queue_ptr) {
	tlock_queue_t* queue = (tlock_queue_t*) queue_ptr;
	int* popped_value;

	/* Read values from queue. Break loop on -1 */
	while(1) {
		if ( (popped_value = tlock_pop(queue)) != NULL ) {

			if (*popped_value == -1) {
				free(popped_value);
				break;
			}

			free(popped_value);
		} 
	}

	thrd_exit(0);
}

int main() {
	int i;					/* for loop index */
	thrd_t push_threads[NUM_PUSH_THREADS];	/* array of push threads */
	thrd_t pop_threads[NUM_POP_THREADS];	/* array of pop threads */
	int* kill_signal;			/* pointer to value that signals pop threads to exit */

	/* Init queue */
	tlock_queue_t* queue = tlock_init();

	/* Start push threads */
	for (i=0; i<NUM_PUSH_THREADS; ++i) {
		if ( thrd_create(&push_threads[i], push_thread, queue) != thrd_success ) {
			printf("Error creating push thread %i\n", i);
		}
	}

	/* Start pop threads */
	for (i=0; i<NUM_POP_THREADS; ++i) {
		if ( thrd_create(&pop_threads[i], pop_thread, queue) != thrd_success ) {
			printf("Error creating pop thread %i\n", i);
		}
	}

	/* Join push threads */
	for (i=0; i<NUM_PUSH_THREADS; ++i) {
		if ( thrd_join(push_threads[i], NULL) != thrd_success )
			continue;
	}

	/* Push kill signals */
	for (i=0; i<NUM_POP_THREADS; ++i) {
		kill_signal = malloc(sizeof(int));
		*kill_signal = -1;
		tlock_push(queue, kill_signal);
	}

	/* Join pop threads */
	for (i=0; i<NUM_POP_THREADS; ++i) {
		if ( thrd_join(pop_threads[i], NULL) != thrd_success )
			continue;
	}

	/* Free queue resources */
	tlock_free(queue);

	return 0;
}

#else

int main() {
	printf("C11 threads are not supported on this system.\n");
	return 1;
}

#endif