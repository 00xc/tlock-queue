# tlock-queue #

tlock-queue is a simple, concurrent, multi-producer, multi-consumer queue written in pure C using [C11 threads' mutexes](https://en.cppreference.com/w/c/thread). It is based on the two lock queue presented by Maged M. Michael and Michael L. Scott in their paper titled [Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf).

## Using and compiling ##
This repository includes an example use in [tlock_test.c](src/tlock_test.c). To compile just run the `make.sh` script. The API consists of 4 functions:
* Queue initialization: `tlock_init()`
* Adding elements to queue: `tlock_push()`
* Remove element from queue: `tlock_pop()`
* Free queue: `tlock_free()`
* Get minimum number of elements in queue at function call: `tlock_length()`

## Speed ##
The main example file, [tlock_test.c](src/tlock_test.c), fires up several push and pop threads (threads writing into or reading from queue respectively), and adds one million integers into the queue from each push thread.

For comparison, source files for a non blocking queue, [lfqueue](https://github.com/Taymindis/lfqueue), are included in the test directory, as well as an analogue example. The `speed.bash` script will run several tests with different amount of push and pop threads, using both tlock-queue and lfqueue in the same manner. It will output the time taken for each one. Sample output:
```
$ ./speed.bash 
push threads: 2, pop threads: 2, tlock=0m1,176s, lfqueue=0m1,574s
push threads: 2, pop threads: 4, tlock=0m1,297s, lfqueue=0m1,912s
push threads: 2, pop threads: 6, tlock=0m1,357s, lfqueue=0m2,485s
push threads: 4, pop threads: 2, tlock=0m1,325s, lfqueue=0m2,927s
push threads: 4, pop threads: 4, tlock=0m1,904s, lfqueue=0m3,500s
push threads: 4, pop threads: 6, tlock=0m2,288s, lfqueue=0m4,030s
```