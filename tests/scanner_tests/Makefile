CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g

SCANNER_SRC = ../../scanner/scanner.c

TEST_TARGET = scanner_tests.exe
TEST_SRC = scanner_tests.c

PRINT_TARGET = scanner_output_print_test.exe
PRINT_SRC = scanner_output_print_test.c

.PHONY: all build test print valgrind clean

all: test

build: $(TEST_TARGET) $(PRINT_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(SCANNER_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) $(SCANNER_SRC) -o $(TEST_TARGET)

$(PRINT_TARGET): $(PRINT_SRC) $(SCANNER_SRC)
	$(CC) $(CFLAGS) $(PRINT_SRC) $(SCANNER_SRC) -o $(PRINT_TARGET)

test: $(TEST_TARGET)
	$(TEST_TARGET) < test_input.ifj26

print: $(PRINT_TARGET)
	$(PRINT_TARGET) < test_input.ifj26

valgrind: $(TEST_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_TARGET) < test_input.ifj26

clean:
	rm -f $(TEST_TARGET) $(PRINT_TARGET)