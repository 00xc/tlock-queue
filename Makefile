CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wpedantic -Wextra -O3 -flto
LDFLAGS=-Isrc -lpthread

BIN_FLAGS=-fPIE
SO_FLAGS=-fPIC -shared

SHARED_LIB=libtlockqueue.so
STATIC_LIB=libtlockqueue.a

.PHONY: all shared static clean

all: test_program shared static

test_program: examples/tlock_test.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(BIN_FLAGS) $< $(STATIC_LIB) $(LDFLAGS) -o $@

shared: $(SHARED_LIB)

static: $(STATIC_LIB)

$(SHARED_LIB): src/tlock_queue.c
	$(CC) $(CFLAGS) $(SO_FLAGS) $^ $(LDFLAGS) -o $@

$(STATIC_LIB): src/tlock_queue.c
	$(CC) $(CFLAGS) -c $^ -o tlock_queue.o
	ar rcs $@ tlock_queue.o

clean:
	rm -f test_program
	rm -f tlock_queue.o
	rm -f $(SHARED_LIB) $(STATIC_LIB)
