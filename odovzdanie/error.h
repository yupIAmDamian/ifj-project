// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)

#ifndef ERROR_H
#define ERROR_H
// error function needs noreturn in its declaration to prevent compiler warnings
#if defined(__GNUC__) || defined(__clang__)
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// error codes
typedef enum {
  ERR_LEX = 1,
  ERR_SYNTAX = 2,
  ERR_UNDEFINED = 3,
  ERR_REDEFINED = 4,
  ERR_ARGS = 5,
  ERR_TYPE = 6,
  ERR_CONST_ASSIGN = 7,
  ERR_OTHER_SEM = 10,
  ERR_INTERNAL = 99
} ErrorCode;

NORETURN void error_exit(ErrorCode code, int line, char *detail);

void error_register_cleanup(void (*fn)(void));

#endif