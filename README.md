# tlock-queue #

tlock-queue is a simple, concurrent, multi-producer, multi-consumer queue implementation in pure C using [C11 threads' mutexes](https://en.cppreference.com/w/c/thread). It is based on the two lock queue presented by Maged M. Michael and Michael L. Scott in their paper titled [Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf).

## Compiling and using ##
To compile the test program ([tlock_test.c](src/tlock_test.c)) just run `make` or `make tlock`. The library can be also compiled as a static (`make static`) or shared (`make shared`) library. No dependencies are needed, just C11 thread support.\
The following code illustrates the use of the public API.

```C
tlock_queue_t* queue;
int* element;

/* Allocate an element to be added to the queue */
element = malloc(sizeof(int));
*element = 420;

/* Queue allocation and initialization. Returns NULL on error */
if ( (queue = tlock_init()) == NULL)
	handle_error();

/* Add the element to the queue. Returns TLOCK_OK on success, TLOCK_ERROR on error */
if (tlock_push(queue, element) != TLOCK_OK)
	handle_error();

/*
 * Get the number of elements in the queue. In a multithreaded environment this function returns
 * a mininum guarantee, but the actual number of elements could be higher. It blocks `tlock_pop()`
 * calls on other threads, but not `tlock_push()` calls.
 */
assert(tlock_min_size(queue) == 1);

/* Retrieve the element from the queue. Returns NULL on empty queue. */
if ( (element = tlock_pop(queue)) == NULL)
	handle_error();

/* This assertion will never fail */
assert(*element == 420);

/* Finally, free the element and the queue */
free(element);
free(queue);
```
