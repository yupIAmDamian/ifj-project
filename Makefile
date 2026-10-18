CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

TESTTOCOMPILE = tests/tests.c tests/hashtable_tests/hashtable_tests.c tests/hashtable_tests/symtable_tests.c hashtable/hashtable.c hashtable/symtable.c helper_func.c 

.PHONY: all build check build-check clean tests clean-tests valgrind

all: build

build: main.c
	$(CC) $(CFLAGS) main.c -o main

check: main.c
	valgrind  $(CC) $(CFLAGS) -fsyntax-only main.c

build-check: main.c
	$(CC) $(CFLAGS) -fsyntax-only main.c

clean:
	rm -f main

tests: $(TESTTOCOMPILE)
	$(CC) $(CFLAGS) -g -O0 $(TESTTOCOMPILE) \
		-o tests/tests
		tests/tests

clean-tests: tests/tests
	rm -f tests/tests

valgrind: tests/tests
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--num-callers=30 \
		--error-exitcode=1 \
		./tests/tests
