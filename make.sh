#!/bin/sh
flags="-lpthread -O3 -flto -std=c11 -fPIE -Wall"

gcc test/lfqueue_test.c test/lfqueue.c -o bin/lfqueue $flags
gcc src/tlock_test.c src/tlock_queue.c -o bin/tlock_queue $flags
