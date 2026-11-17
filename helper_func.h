// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./scanner/scanner.h"

char* str_duplicate(char *str);
FILE *open_file(char *filename, char *mode);

void dominik_error(int code, const char *message);

char *token_type_name(TokenType type);

#endif