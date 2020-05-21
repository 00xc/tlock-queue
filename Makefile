CC=gcc
CFLAGS=-O3 -flto -std=c11 -fPIE -Wall -Wpedantic
LDFLAGS=-lpthread

tlock: src/*.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o bin/tlock_queue

test: test/*.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o bin/lfqueue

all: tlock test

clean:
	rm -f bin/*