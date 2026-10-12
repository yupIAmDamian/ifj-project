CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

all: main

main: tests.c
	$(CC) $(CFLAGS) tests.c -o tests

clean:
	rm -f tests