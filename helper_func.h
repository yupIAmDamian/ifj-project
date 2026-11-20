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

char *token_type_name(TokenType type);

bool check_if_string_is_not_in_array(char *str, char **array, size_t array_size);

#endif