#!/bin/bash

push_threads=(2 4)
pop_threads=(2 4 6)

for push in "${push_threads[@]}"; do

	# Update number of push threads
	sed -i "s/NUM_PUSH_THREADS .*/NUM_PUSH_THREADS ${push}/g" test/lfqueue_test.c
	sed -i "s/NUM_PUSH_THREADS .*/NUM_PUSH_THREADS ${push}/g" src/tlock_test.c

	for pop in "${pop_threads[@]}"; do

		# Update number of pop threads
		sed -i "s/NUM_POP_THREADS .*/NUM_POP_THREADS ${pop}/g" test/lfqueue_test.c
		sed -i "s/NUM_POP_THREADS .*/NUM_POP_THREADS ${pop}/g" src/tlock_test.c

		# Compile and run both tests
		sh make.sh
		tlock=$({ time ./bin/tlock_queue; } 2>&1 | grep real | awk '{print $2}')
		lfqueue=$({ time ./bin/lfqueue; } 2>&1 | grep real | awk '{print $2}')

		echo "push threads: ${push}, pop threads: ${pop}, tlock=${tlock}, lfqueue=${lfqueue}"

	done
done
