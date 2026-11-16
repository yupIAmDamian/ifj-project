#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../syntactic_analysis/parser.h"

extern ASTNode *global_ast_root;

#define CHECK(condition, message)              \
    do {                                       \
        if (!(condition)) {                    \
            printf("[CHYBA] %s\n", message);   \
            failures++;                        \
        }                                      \
    } while (0)

int main(int argc, char *argv[]) {

    int failures = 0;

    printf("--- Spustam testy pre parser.c ---\n");

    if (argc < 3 || argc > 5) {
        printf("Pouzitie: ./test_parser <subor.ifj> <ocakavany_vysledok> [is_main]\n");
        printf("Ocakavany vysledok: 0 = uspech, 1 = lexical error, 2 = syntax error\n");
        return 1;
    }

    if (freopen(argv[1], "r", stdin) == NULL) {
        printf("[CHYBA] Nepodarilo sa otvorit vstupny subor: %s\n", argv[1]);
        return 1;
    }

    char *endptr;
    long expected_result = strtol(argv[2], &endptr, 10);

    if (*endptr != '\0' || expected_result < 0 || expected_result > 2) {
        printf("[CHYBA] Nespravny ocakavany vysledok\n");
        return 1;
    }

    int expected_main = -1;

    if (argc == 4) {
        expected_main = (int)strtol(argv[3], &endptr, 10);

        if (*endptr != '\0' || (expected_main != 0 && expected_main != 1)) {
            printf("[CHYBA] Nespravna hodnota is_main\n");
            return 1;
        }
    }

    int expected_const = -1;

    if (argc == 5) {
        expected_const = (int)strtol(argv[4], &endptr, 10);
        if (*endptr != '\0' || (expected_const != 0 && expected_const != 1)) {
            printf("[CHYBA] Nespravna hodnota is_const\n");
            return 1;
        }
    }

    printf("\nTest 1: parse() - parsovanie vstupneho suboru\n");

    int result = parse();

    if (result != expected_result) {
        printf("[CHYBA] Ocakavany navratovy kod: %ld, dostali sme: %d\n",
               expected_result, result);
        failures++;
    }
    else {
        printf("[OK] Navratovy kod parse() je spravny: %d\n", result);
    }

    printf("\nTest 2: Kontrola AST root uzla\n");

    if (result == 0) {

        CHECK(global_ast_root != NULL,
              "AST root uzol nebol vytvoreny");

        if (global_ast_root != NULL) {

            CHECK(global_ast_root->type == AST_NODE_PROGRAM,
                  "Root uzol nema typ AST_NODE_PROGRAM");

            if (global_ast_root->type == AST_NODE_PROGRAM) {
                printf("[OK] AST root uzol bol spravne vytvoreny\n");
            }
        }
    }
    else {
        CHECK(global_ast_root == NULL,
              "AST root mal byt NULL pri neuspesnom parsovani");
    }

    if (result == 0 && expected_main != -1) {

        printf("\nTest 3: Kontrola hlavnej funkcie\n");

        if (global_ast_root != NULL && global_ast_root->left != NULL) {

            ASTNode *function_node = global_ast_root->left;

            CHECK(function_node->type == AST_NODE_FUNCTION,
                  "Prvy uzol v programe nie je AST_NODE_FUNCTION");

            CHECK(function_node->is_main == (expected_main == 1),
                  "Nespravna hodnota is_main");

            if (function_node->type == AST_NODE_FUNCTION &&
                function_node->is_main == (expected_main == 1)) {
                printf("[OK] Prvy function uzol je spravne inicializovany\n");
            }
        }
        else {
            printf("[CHYBA] Program neobsahuje ziadnu funkciu\n");
            failures++;
        }
    }

    if (result == 0 && expected_const != -1) {
        printf("\nTest 4: Kontrola deklaracie premennej (is_const)\n");

        if (global_ast_root != NULL &&
            global_ast_root->left != NULL &&
            global_ast_root->left->right != NULL) {

            ASTNode *stmt_node = global_ast_root->left->right;

            CHECK(stmt_node->type == AST_NODE_VAR_DECL,
                  "Prvy prikaz vo funkcii nie je AST_NODE_VAR_DECL");

            CHECK(stmt_node->is_const == (expected_const == 1),
                  "Nespravna hodnota is_const na uzle AST_NODE_VAR_DECL");

            if (stmt_node->left != NULL) {
                CHECK(stmt_node->left->is_const == (expected_const == 1),
                      "Nespravna hodnota is_const na vnorenom uzle AST_NODE_VAR");
            }

            if (stmt_node->type == AST_NODE_VAR_DECL &&
                stmt_node->is_const == (expected_const == 1)) {
                printf("[OK] Deklaracia premennej ma spravne nastaveny is_const = %d\n", expected_const);
            }
        }
        else {
            printf("[CHYBA] Funkcia neobsahuje ziadny prikaz na otestovanie is_const\n");
            failures++;
        }
    }

    parser_cleanup();

    printf("\n--- Vysledok testov ---\n");

    if (failures == 0) {
        printf("[OK] Vsetky testy presli!\n");
    }
    else {
        printf("[CHYBA] Pocet neuspesnych kontrol: %d\n", failures);
    }

    printf("--- Koniec testov ---\n");

    return failures == 0 ? 0 : 1;
}
