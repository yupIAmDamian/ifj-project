CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

all: build

build: main.c
	$(CC) $(CFLAGS) main.c -o main

check: main.c
	valgrind  $(CC) $(CFLAGS) -fsyntax-only main.c

build-check: main.c
	$(CC) $(CFLAGS) -fsyntax-only main.c

clean:
	rm -f main