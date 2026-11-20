// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./scanner/scanner.h"

/**
 * @brief Allocates memory and creates a duplicate of a string
 * @param str Pointer to the source string to duplicate.
 * @return Pointer to the newly allocated copy, or NULL if str is NULL or allocation fails.
 */
char* str_duplicate(char *str);

/**
 * @brief Opens a file with the specified mode and terminates the program on failure.
 * @param filename File path.
 * @param mode File access mode.
 * @return Pointer to the opened FILE stream.
 */
FILE *open_file(char *filename, char *mode);

/**
 * @brief Converts a TokenType value into its string representation.
 * @param type TokenType value to convert.
 * @return String naming the token type, or "UNKNOWN_TOKEN" if unrecognized.
 */
char *token_type_name(TokenType type);

/**
 * @brief Checks whether a given string is absent from an array of strings.
 * @param str String to search for.
 * @param array Array of string pointers.
 * @param array_size Number of elements in the array.
 * @return True if the string is not found in the array, false otherwise.
 */
bool check_if_string_is_not_in_array(char *str, char **array, size_t array_size);

#endif