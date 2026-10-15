#include "helper_func.h"

//This function is used to duplicate a string. 
char* str_duplicate(char *str) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL; 
    }
    strcpy(copy, str);

    return copy;
}