# Compiler of IFJ26 imperative language
# Author: Damian Frantisek Girasek (xgirasd00)

CC = gcc
#Compiler flags - check which are really needed and remove the rest
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -I . -I codegen -I syntactic_analysis -I scanner -I hashtable

TESTTOCOMPILE = tests/tests.c tests/hashtable_tests/hashtable_tests.c tests/hashtable_tests/symtable_tests.c hashtable/hashtable.c hashtable/symtable.c helper_func.c tests/codegen_tests/codegen_tests.c codegen/codegen.c codegen/gen_func.c codegen/codegen_helper.c syntactic_analysis/ast.c syntactic_analysis/expression.c syntactic_analysis/parser.c scanner/scanner.c

TRANSLATOR_TESTTOCOMPILE = tests/translator_test/translator_test.c semantic_analysis/semantic.c hashtable/hashtable.c hashtable/symtable.c helper_func.c codegen/codegen.c codegen/gen_func.c codegen/codegen_helper.c syntactic_analysis/ast.c syntactic_analysis/expression.c syntactic_analysis/parser.c scanner/scanner.c

.PHONY: all build check build-check clean tests clean-tests translator-test valgrind

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

#tests for singular input file where we can test only one file from bunch
translator-test: $(TRANSLATOR_TESTTOCOMPILE)
	$(CC) $(CFLAGS) -g -O0 $(TRANSLATOR_TESTTOCOMPILE) \
		-o tests/translator_test/translator_test
	./tests/translator_test/translator_test \
		tests/translator_test/translator_input.ifj26 \
		tests/translator_test/translator_output.ifjcode

clean-translator-test: tests/translator_test/translator_test
	rm -f tests/translator_test/translator_test

valgrind-translator-test: tests/translator_test/translator_test
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--num-callers=30 \
		--error-exitcode=1 \
		./tests/translator_test/translator_test \
		tests/translator_test/translator_input.ifj26 \
		tests/translator_test/translator_output.ifjcode


#Custom script for running translator tests with the interpreter, you can set the path to the interpreter binary using the ICINT variable, e.g.:
ICINT ?= ./ic26int-linux-x86_64

translator-batch: $(TRANSLATOR_TESTTOCOMPILE)
	$(CC) $(CFLAGS) -g -O0 $(TRANSLATOR_TESTTOCOMPILE) \
		-o tests/translator_test/translator_test
	INTERPRETER=$(ICINT) bash tests/translator_test/translator_test.sh