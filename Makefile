# Compiler of IFJ26 imperative language
# Author: Damian Frantisek Girasek (xgirasd00)

CC = gcc
#Compiler flags - check which are really needed and remove the rest
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -I . -I codegen -I syntactic_analysis -I scanner -I hashtable

TESTTOCOMPILE = tests/tests.c tests/hashtable_tests/hashtable_tests.c tests/hashtable_tests/symtable_tests.c hashtable/hashtable.c hashtable/symtable.c helper_func.c tests/codegen_tests/codegen_tests.c codegen/codegen.c codegen/gen_func.c codegen/codegen_helper.c syntactic_analysis/ast.c syntactic_analysis/expression.c syntactic_analysis/parser.c scanner/scanner.c error.c

TRANSLATOR_TESTTOCOMPILE = tests/translator_test/translator_test.c semantic_analysis/semantic.c hashtable/hashtable.c hashtable/symtable.c helper_func.c codegen/codegen.c codegen/gen_func.c codegen/codegen_helper.c syntactic_analysis/ast.c syntactic_analysis/expression.c syntactic_analysis/parser.c scanner/scanner.c error.c

VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=30 --error-exitcode=1

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

#####################################################
# Tests
#####################################################
#Hashtable tests
#####################################################
hashtable-tests: tests/hashtable_tests/hashtable_tests.c hashtable/hashtable.c helper_func.c
	$(CC) $(CFLAGS) -g -O0 tests/hashtable_tests/hashtable_tests.c hashtable/hashtable.c helper_func.c \
		-o tests/hashtable_tests/hashtable_tests
	./tests/hashtable_tests/hashtable_tests

valgrind-hashtable-tests: tests/hashtable_tests/hashtable_tests.c hashtable/hashtable.c helper_func.c
	valgrind $(VALGRIND_FLAGS) \
			./tests/hashtable_tests/hashtable_tests

clean-hashtable-tests: tests/hashtable_tests/hashtable_tests
	rm -f tests/hashtable_tests/hashtable_tests

#####################################################
#Symtable tests
#####################################################
symtable-tests: tests/hashtable_tests/symtable_tests.c hashtable/symtable.c hashtable/hashtable.c helper_func.c
	$(CC) $(CFLAGS) -g -O0 tests/hashtable_tests/symtable_tests.c hashtable/symtable.c hashtable/hashtable.c helper_func.c \
		-o tests/hashtable_tests/symtable_tests
	./tests/hashtable_tests/symtable_tests

valgrind-symtable-tests: tests/hashtable_tests/symtable_tests.c hashtable/symtable.c hashtable/hashtable.c helper_func.c
	valgrind $(VALGRIND_FLAGS) \
			./tests/hashtable_tests/symtable_tests

clean-symtable-tests: tests/hashtable_tests/symtable_tests
	rm -f tests/hashtable_tests/symtable_tests


#Symtable tests

tests: $(TESTTOCOMPILE)
	$(CC) $(CFLAGS) -g -O0 $(TESTTOCOMPILE) \
		-o tests/tests
	./tests/tests

clean-tests: tests/tests
	rm -f tests/tests

valgrind: tests/tests
	valgrind $(VALGRIND_FLAGS) \
		./tests/tests

SEM_TEST_BIN = tests/semantic_tests/test_sem
SEM_TEST_SRCS = tests/semantic_tests/test_semantic.c semantic_analysis/semantic.c syntactic_analysis/ast.c hashtable/symtable.c hashtable/hashtable.c helper_func.c error.c

test-sem:
	gcc -g -O0 -Wall -Wextra -Isemantic_analysis -Isyntactic_analysis -Ihashtable -I. $(SEM_TEST_SRCS) -o $(SEM_TEST_BIN)
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


#####################################################
#Translator tests with interpreter
#####################################################
ICINT ?= ./ic26int-linux-x86_64

translator-batch: $(TRANSLATOR_TESTTOCOMPILE)
	$(CC) $(CFLAGS) -g -O0 $(TRANSLATOR_TESTTOCOMPILE) \
		-o tests/translator_test/translator_test
	INTERPRETER=$(ICINT) bash tests/translator_test/translator_test.sh