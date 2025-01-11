#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "tlock_queue.h"

#define NUM_PUSH_THREADS 4
#define NUM_POP_THREADS 4
#define NUMS 1000000

#define ARR_SIZE(_arr) (sizeof(_arr)/sizeof(_arr[0]))

/* This thread writes integers into the queue */
int push_thread(void* queue_ptr) {
	tlock_queue_t* queue = (tlock_queue_t*) queue_ptr;
	int* pushed_value;
	int i;

	/* Push ints into queue */
	for (i = 0; i < NUMS; ++i) {
		pushed_value = malloc(sizeof(int));
		*pushed_value = i;
		if (tlock_push(queue, pushed_value) != TLOCK_OK ) {
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
		popped_value = tlock_pop(queue);
		if (!popped_value)
			continue;

		if (*popped_value == -1) {
			free(popped_value);
			break;
		}

		free(popped_value);
	}

	thrd_exit(0);
}

int main() {
	int rc;
	unsigned int i;
	thrd_t push_threads[NUM_PUSH_THREADS];
	thrd_t pop_threads[NUM_POP_THREADS];
	int* kill_signal;

	/* Init queue */
	tlock_queue_t* queue = tlock_init();

	/* Start push threads */
	for (i = 0; i < ARR_SIZE(push_threads); ++i) {
		rc = thrd_create(&push_threads[i], push_thread, queue);
		if (rc != thrd_success)
			err(EXIT_FAILURE, "thrd_create(): push_threads[%i]", i);
	}

	/* Start pop threads */
	for (i = 0; i < ARR_SIZE(pop_threads); ++i) {
		rc = thrd_create(&pop_threads[i], pop_thread, queue);
		if (rc != thrd_success)
			err(EXIT_FAILURE, "thrd_create(): pop_threads[%i]", i);
	}

	/* Join push threads */
	for (i = 0; i < ARR_SIZE(push_threads); ++i) {
		rc = thrd_join(push_threads[i], NULL);
		if (rc != thrd_success)
			warn("thrd_join(): push_threads[%i]", i);
	}

	/* Push kill signals */
	for (i = 0; i < ARR_SIZE(pop_threads); ++i) {
		kill_signal = malloc(sizeof(int));
		if (!kill_signal)
			err(EXIT_FAILURE, "malloc");
		*kill_signal = -1;
		tlock_push(queue, kill_signal);
	}

	/* Join pop threads */
	for (i = 0; i < ARR_SIZE(pop_threads); ++i) {
		rc = thrd_join(pop_threads[i], NULL);
		if (rc != thrd_success)
			warn("thrd_join(): pop_threads[%i]", i);
	}

	tlock_free(queue);
	return 0;
}
