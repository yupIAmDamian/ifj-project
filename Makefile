CC = gcc
#Compiler flags - check which are really needed and remove the rest
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -I . -I codegen -I syntactic_analysis -I scanner -I hashtable

TESTTOCOMPILE = tests/tests.c tests/hashtable_tests/hashtable_tests.c tests/hashtable_tests/symtable_tests.c hashtable/hashtable.c hashtable/symtable.c helper_func.c tests/codegen_tests/codegen_tests.c codegen/codegen.c codegen/gen_func.c codegen/codegen_helper.c syntactic_analysis/ast.c syntactic_analysis/expression.c syntactic_analysis/parser.c scanner/scanner.c
TESTOUTPUT = tests/codegen_tests/codegen_output.txt

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
	./tests/tests


clean-tests: tests/tests
	rm -f tests/tests

valgrind: tests/tests
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--num-callers=30 \
		--error-exitcode=1 \
		./tests/tests
