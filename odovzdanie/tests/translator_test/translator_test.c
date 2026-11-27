// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#include "translator_test.h"
#include "../../codegen.h"
#include "../../symtable.h"
#include "../../helper_func.h"
#include "../../semantic.h"
#include "../../parser.h"

extern ASTNode *global_ast_root;
extern Token current_token;

// symtable
static Symtable *st = NULL;

// function for freeing the symtable, registered in atexit
static void symtable_cleanup(void) {
  symtable_dispose(st);
  st = NULL;
}

int redirect_streams(char *input_file, char *output_file) {
  if (freopen(input_file, "r", stdin) == NULL) {
    fprintf(stderr, "Cannot open input file: %s\n", input_file);
    return 1;
  }

  if (freopen(output_file, "w", stdout) == NULL) {
    fprintf(stderr, "Cannot open output file: %s\n", output_file);
    return 1;
  }

  return 0;
}

int translator_test(char *input_file, char *output_file) {
  error_register_cleanup(symtable_cleanup); // danko
  error_register_cleanup(ast_cleanup);      // misko
  error_register_cleanup(
      expr_stack_cleanup); // miskov stack free wrapnuty, odkomentuj ked bude
  error_register_cleanup(scanner_free); // dominik

  if (redirect_streams(input_file, output_file) != 0) {
    return 1;
  }

  int parse_result = parse();
  if (parse_result != 0) {
    fprintf(stderr, "Parsing failed: code=%d, token=%s (%d), line=%d\n",
            parse_result, token_type_name(current_token.type),
            current_token.type, current_token.line);

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
  fflush(stdout);
  symtable_dispose(symtable);

  fprintf(stderr, "Translation succeeded: %s -> %s\n", input_file, output_file);
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input.ifj26> <output.ifjcode>\n", argv[0]);
    return 2;
  }

  return translator_test(argv[1], argv[2]);
}