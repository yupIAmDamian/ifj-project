#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h" //this should be syntax tree from Miško, but i dont know where to find it
#include "symtable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//main and only function i believe
bool check_semantics(ASTNode *root, Symtable *st);

#endif