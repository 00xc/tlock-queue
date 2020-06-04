CC=gcc
CFLAGS=-O3 -flto -std=c11 -Wall -Wpedantic
LDFLAGS=-lpthread

BIN_FLAGS=-fPIE
SO_FLAGS=-fPIC -shared

tlock: src/*.c
	$(CC) $(CFLAGS) $(BIN_FLAGS) $(LDFLAGS) $^ -o bin/tlock_queue

test: test/*.c
	$(CC) $(CFLAGS) $(BIN_FLAGS) $(LDFLAGS) $^ -o bin/lfqueue

shared: src/tlock_queue.c
	$(CC) $(CFLAGS) $(SO_FLAGS) $(LDFLAGS) $^ -o bin/tlock_queue.so

static: src/tlock_queue.c
	gcc -c $^ -o bin/tlock_queue.o
	ar rcs bin/tlock_queue.a bin/tlock_queue.o

all: tlock test shared

clean:
	rm -f bin/*