// Compiler of IFJ26 imperative language
// Authors: Damian Frantisek Girasek (xgirasd00), Daniel Jurkovič (xjurkod00),
//          Dominik Madaj (xmadajd00), Michal Kopček (xkopcem00)
/*
#include "scanner.h"
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "expression.h"
#include "parser.h"
#include "codegen.h"
*/
#include "main.h"
#include "./error.h"

#include "./scanner.h"
#include "./ast.h"
#include "./expression.h"

#include "./codegen.h"
#include "./symtable.h"
#include "./semantic.h"
#include "./parser.h"
#include "./helper_func.h"

extern ASTNode *global_ast_root;
extern Token current_token;

// symtable
static Symtable *st = NULL;

// function for freeing the symtable, registered in atexit
static void symtable_cleanup(void) {
    symtable_dispose(st);
    st = NULL;
}


int main() {
    // registers all cleanup functions
    error_register_cleanup(symtable_cleanup); // danko
    error_register_cleanup(ast_cleanup); // misko
    // error_register_cleanup(expr_stack_cleanup); // miskov stack free wrapnuty, odkomentuj ked bude
    error_register_cleanup(scanner_free); // dominik

    scanner_init();

    // HLAVNA CAST PROGRAMU
    int parse_result = parse();
    if (parse_result != 0) {
        fprintf(stderr,
                "Parsing failed: code=%d, token=%s (%d), line=%d\n",
                parse_result,
                token_type_name(current_token.type),
                current_token.type,
                current_token.line);
        return parse_result;
    }

    if (global_ast_root->left != NULL && global_ast_root->left->is_main) {
        global_ast_root->left->inferred_type = TYPE_VOID;
    }

    Symtable *symtable = symtable_init();
    if (symtable == NULL) {
        fprintf(stderr, "Could not initialize symbol table\n");
        return 99;
    }

    if (!check_semantics(global_ast_root, symtable)) {
        symtable_dispose(symtable);
        fprintf(stderr, "Semantic analysis failed\n");
        return 1;
    }

    gen_code(global_ast_root);

    symtable_dispose(symtable);

    return 0;
}