// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)

#include "error.h"

#define MAX_CLEANUPS 16 // we probably dont need this many

static void (*cleanups[MAX_CLEANUPS])(void);
static int cleanup_count = 0;
static bool atexit_registered = false;

// function for printing the corresponding error message
static const char *error_message(ErrorCode code) {
    switch (code) {
        case ERR_LEX: return "lexical error";
        case ERR_SYNTAX: return "syntax error";
        case ERR_UNDEFINED: return "undefined function, variable or type";
        case ERR_REDEFINED: return "redefinition";
        case ERR_ARGS: return "wrong number or type of arguments";
        case ERR_TYPE: return "type error";
        case ERR_CONST_ASSIGN: return "assignment to an immutable variable";
        case ERR_OTHER_SEM: return "semantic error";
        case ERR_INTERNAL: return "internal compiler error";
        default: return "unknown error";
    }
}

// function for calling the cleanup functions when exiting the program
static void run_cleanups(void) {
    while (cleanup_count > 0) {
        cleanup_count--;
        cleanups[cleanup_count]();
    }
}

// function for storing the addresses of cleanup functions in an array
// registers run_cleanups in atexit
void error_register_cleanup(void (*fn)(void)) {
    if (!atexit_registered) {
        atexit(run_cleanups);
        atexit_registered = true;
    }
    if (cleanup_count < MAX_CLEANUPS) {
        cleanups[cleanup_count] = fn;
        cleanup_count++;
    }
}

// function for exiting the program in case of an error
void error_exit(ErrorCode code, int line, const char *detail) {
    // prints all details on stderr
    fprintf(stderr, "Error %d", code);
    if (line > 0) {
        fprintf(stderr, " (line %d)", line);
    }
    fprintf(stderr, ": %s", error_message(code));
    if (detail != NULL) {
        fprintf(stderr, " - %s", detail);
    }
    fputc('\n', stderr);
    exit(code);
}