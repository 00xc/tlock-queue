CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wpedantic -Wextra -O3 -flto
LDFLAGS=-lpthread

BIN_FLAGS=-fPIE
SO_FLAGS=-fPIC -shared

SHARED_LIB=libtlockqueue.so
STATIC_LIB=libtlockqueue.a

.PHONY: all tlock shared static clean

tlock: src/tlock_test.c static
	$(CC) $(CFLAGS) $(BIN_FLAGS) $< $(STATIC_LIB) $(LDFLAGS) -o test_program.bin

shared: $(SHARED_LIB)

static: $(STATIC_LIB)

all: tlock shared static

$(SHARED_LIB): src/tlock_queue.c
	$(CC) $(CFLAGS) $(SO_FLAGS) $^ $(LDFLAGS) -o $@

$(STATIC_LIB): src/tlock_queue.c
	$(CC) $(CFLAGS) -c $^ -o tlock_queue.o
	ar rcs $@ tlock_queue.o

clean:
	rm -f test_program.bin
	rm -f tlock_queue.o $(SHARED_LIB) $(STATIC_LIB)
