// Compiler of IFJ 2026 imperative language
// Author: Daniel Jurkovič (xjurkod00)

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "error.h"
#include "scanner.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Entry point for semantic analysis; registers built-in and user-defined
 * functions and validates types, scopes, and semantic rules across the entire
 * AST.
 * @param root Root node of the Abstract Syntax Tree.
 * @param st Pointer to the initialized symbol table.
 * @return true if semantic analysis succeeds without errors.
 */
bool check_semantics(ASTNode *root, Symtable *st);

#endif