CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wpedantic -Wextra -O3 -flto
LDFLAGS=-lpthread

BIN_FLAGS=-fPIE
SO_FLAGS=-fPIC -shared

.PHONY: all tlock shared static clean

tlock: src/*.c
	$(CC) $(CFLAGS) $(BIN_FLAGS) $^ $(LDFLAGS) -o test_program.bin

shared: src/tlock_queue.c
	$(CC) $(CFLAGS) $(SO_FLAGS) $^ $(LDFLAGS) -o tlock_queue.so

static: src/tlock_queue.c
	$(CC) $(CFLAGS) -c $^ -o tlock_queue.o
	ar rcs tlock_queue.a tlock_queue.o

all: tlock shared static

clean:
	rm -f test_program.bin
	rm -f tlock_queue.so
	rm -f tlock_queue.o
	rm -f tlock_queue.a
