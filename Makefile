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

all: tlock test shared

clean:
	rm -f bin/*