CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wpedantic -Wextra -O3 -flto -Iinclude/
LDFLAGS=-lpthread

BIN_FLAGS=-fPIE
SO_FLAGS=-fPIC -shared

OBJS=tlock_queue.o
SHARED_LIB=libtlockqueue.so
STATIC_LIB=libtlockqueue.a

.PHONY: all shared static clean

all: test_program shared static

shared: $(SHARED_LIB)

static: $(STATIC_LIB)

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(SHARED_LIB): $(OBJS)
	$(CC) $(CFLAGS) $(SO_FLAGS) -o $@ $^ $(LDFLAGS)

$(STATIC_LIB): $(OBJS)
	$(AR) rcs $@ $^

test_program: examples/tlock_test.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(BIN_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f test_program
	rm -f $(OBJS) $(SHARED_LIB) $(STATIC_LIB)
