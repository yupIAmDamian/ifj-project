TARGET = test_scanner.exe

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g

SCANNER_SRC = ../../scanner/scanner.c

TEST_SRC = scanner_tests.c

all: build run

build:
	$(CC) $(CFLAGS) $(TEST_SRC) $(SCANNER_SRC) -o $(TARGET)

run: build
	$(TARGET) < test_input.ifj26

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET) test_input.tmp