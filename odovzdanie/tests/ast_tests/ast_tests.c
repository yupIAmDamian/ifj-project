#include "../../syntactic_analysis/ast.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int failures = 0;

void CHECK(bool condition, char *message) {
  if (!condition) {
    printf("[CHYBA] %s\n", message);
    failures++;
  }
}

int main() {

  int failures_before = 0;

  printf("--- Spustam testy pre ast.c ---\n");

  printf("\nTest 1: create_ast_node() zakladna inicializacia\n");

  Token dummy_token = {0};
  dummy_token.type = TOKEN_INTEGER;

  ASTNode dummy_left = {0};
  ASTNode dummy_right = {0};
  ASTNode dummy_extra = {0};

  failures_before = failures;

  ASTNode *node = create_ast_node(AST_NODE_TERM, &dummy_left, &dummy_right,
                                  &dummy_extra, dummy_token);

  if (node == NULL) {
    printf("[CHYBA] Alokacia uzla zlyhala\n");
    failures++;
  } else {

    CHECK(node->type == AST_NODE_TERM, "Nespravny typ uzla");

    CHECK(node->left == &dummy_left,
          "Ukazovatel left neukazuje na spravny uzol");

    CHECK(node->right == &dummy_right,
          "Ukazovatel right neukazuje na spravny uzol");

    CHECK(node->extra == &dummy_extra,
          "Ukazovatel extra neukazuje na spravny uzol");

    CHECK(node->next == NULL, "Ukazovatel next mal byt NULL");

    CHECK(node->prev == NULL, "Ukazovatel prev mal byt NULL");

    CHECK(node->is_main == false, "is_main mal byt defaultne false");

    CHECK(node->is_const == false, "is_const mal byt defaultne false");

    CHECK(node->val.type == dummy_token.type, "Token nebol spravne priradeny");

    free(node);
  }

  if (failures == failures_before) {
    printf("[OK] Test 1 uspesny!\n");
  }

  printf("\nTest 2: create_ast_node() bez potomkov (NULL)\n");

  Token token_var = {0};
  token_var.type = TOKEN_IDENTIFIER;

  failures_before = failures;

  ASTNode *leaf_node =
      create_ast_node(AST_NODE_VAR, NULL, NULL, NULL, token_var);

  if (leaf_node == NULL) {
    printf("[CHYBA] Alokacia uzla zlyhala\n");
    failures++;
  } else {

    CHECK(leaf_node->type == AST_NODE_VAR, "Nespravny typ uzla");

    CHECK(leaf_node->left == NULL, "left mal byt NULL");

    CHECK(leaf_node->right == NULL, "right mal byt NULL");

    CHECK(leaf_node->extra == NULL, "extra mal byt NULL");

    CHECK(leaf_node->next == NULL, "next mal byt NULL");

    CHECK(leaf_node->prev == NULL, "prev mal byt NULL");

    CHECK(leaf_node->is_main == false, "is_main mal byt defaultne false");

    CHECK(leaf_node->is_const == false, "is_const mal byt defaultne false");

    CHECK(leaf_node->val.type == token_var.type,
          "Token nebol spravne priradeny");

    free(leaf_node);
  }

  if (failures == failures_before) {
    printf("[OK] Test 2 uspesny!\n");
  }

  printf("\n--- Vysledok testov ---\n");

  if (failures == 0) {
    printf("[OK] Vsetky testy presli!\n");
  } else {
    printf("[CHYBA] Pocet neuspesnych kontrol: %d\n", failures);
  }

  printf("--- Koniec testov ---\n");

  return failures == 0 ? 0 : 1;
}
