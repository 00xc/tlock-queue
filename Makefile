CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wpedantic -Wextra -Ofast -flto
LDFLAGS=-lpthread

BIN_FLAGS=-fPIE
SO_FLAGS=-fPIC -shared

SHARED_LIB=libtlockqueue.so
STATIC_LIB=libtlockqueue.a

.PHONY: all tlock shared static clean

tlock: src/*.c
	$(CC) $(CFLAGS) $(BIN_FLAGS) $^ $(LDFLAGS) -o test_program.bin

shared: src/tlock_queue.c
	$(CC) $(CFLAGS) $(SO_FLAGS) $^ $(LDFLAGS) -o $(SHARED_LIB)

static: src/tlock_queue.c
	$(CC) $(CFLAGS) -c $^ -o tlock_queue.o
	ar rcs $(STATIC_LIB) tlock_queue.o

all: tlock shared static

clean:
	rm -f test_program.bin
	rm -f tlock_queue.o
	rm -f $(SHARED_LIB)
	rm -f $(STATIC_LIB)
