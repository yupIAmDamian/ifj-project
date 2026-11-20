# Compiler of IFJ26 imperative language
# Authors: Damian Frantisek Girasek (xgirasd00), Dominik Madaj (xmadajd00)

CC = gcc

INCLUDES = -I . -I codegen -I syntactic_analysis -I semantic_analysis -I scanner -I hashtable
#Compiler flags - check which are really needed and remove the rest
CFLAGS   = -std=c11 -Wall -Wextra -pedantic -g $(INCLUDES)

TARGET = main

# all .c files for the project
SRCS = \
	scanner.c \
	ast.c \
	expression.c \
	parser.c \
	semantic.c \
	codegen.c \
	gen_func.c \
	codegen_helper.c \
	hashtable.c \
	symtable.c \
	helper_func.c \
	error.c

.PHONY: all build check build-check format clean

all: build

build:
	$(CC) $(CFLAGS) main.c $(SRCS) -o $(TARGET)


check:
	cppcheck --enable=warning,style --error-exitcode=1 --quiet $(INCLUDES) main.c $(SRCS)

build-check: check build


# do wee need this
# format:
# 	clang-format -i $$(git ls-files '*.c' '*.h')

clean:
	rm -f $(TARGET) $(UNIT_TESTS)

#####################################################
# Tests
#####################################################
# each test = individual program

HASHTABLE_T  = tests/hashtable_tests/hashtable_tests
SYMTABLE_T   = tests/hashtable_tests/symtable_tests
SCANNER_T    = tests/scanner_tests/scanner_tests
SCANNER_PR_T = tests/scanner_tests/scanner_output_print_test
AST_T        = tests/ast_tests/ast_tests
EXPRESSION_T = tests/expression_tests/expression_tests
PARSER_T     = tests/parser_tests/parser_tests
SEMANTIC_T   = tests/semantic_tests/test_semantic
CODEGEN_T    = tests/codegen_tests/codegen_tests
TRANSLATOR_T = tests/translator_test/translator_test

UNIT_TESTS = $(HASHTABLE_T) $(SYMTABLE_T) $(SCANNER_T) $(SCANNER_PR_T) \
             $(AST_T) $(EXPRESSION_T) $(PARSER_T) $(SEMANTIC_T) $(CODEGEN_T) $(TRANSLATOR_T)

$(UNIT_TESTS): %: %.c $(SRCS)
	$(CC) $(CFLAGS) -O0 $< $(SRCS) -o $@


# --- Run tests ---
# for valgrind: make test-xxxxx RUN="valgrind --..."
RUN ?=
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --num-callers=30 --error-exitcode=1

.PHONY: test-hashtable test-symtable test-scanner test-scanner-print test-ast \
        test-expression test-parser test-semantic test-codegen test-translator \
        test-all valgrind translator-batch

test-hashtable: $(HASHTABLE_T)
	$(RUN) ./$(HASHTABLE_T)

test-symtable: $(SYMTABLE_T)
	$(RUN) ./$(SYMTABLE_T)

test-scanner: $(SCANNER_T)
	$(RUN) ./$(SCANNER_T) < tests/scanner_tests/test_input.ifj26

test-scanner-print: $(SCANNER_PR_T)
	$(RUN) ./$(SCANNER_PR_T) < tests/scanner_tests/test_input.ifj26

test-ast: $(AST_T)
	$(RUN) ./$(AST_T)

test-expression: $(EXPRESSION_T)
	$(RUN) ./$(EXPRESSION_T)

test-parser: $(PARSER_T)
	$(RUN) ./$(PARSER_T)

test-semantic: $(SEMANTIC_T)
	$(RUN) ./$(SEMANTIC_T)

test-codegen: $(CODEGEN_T)
	$(RUN) ./$(CODEGEN_T)

test-translator: $(TRANSLATOR_T)
	$(RUN) ./$(TRANSLATOR_T) \
		tests/translator_test/translator_input.ifj26 \
		tests/translator_test/translator_output.ifjcode

test-all: test-hashtable test-symtable test-scanner test-ast test-expression \
          test-parser test-semantic test-codegen test-translator

# all tests + valgrind
valgrind:
	$(MAKE) test-all RUN="$(VALGRIND)"

ICINT ?= ./ic26int-linux-x86_64

translator-batch: $(TRANSLATOR_T)
	INTERPRETER=$(ICINT) bash tests/translator_test/translator_test.sh