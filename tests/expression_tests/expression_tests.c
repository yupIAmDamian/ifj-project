#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../syntactic_analysis/expression.h"

int failures = 0;

Token current_token;

static const Token *input_tokens = NULL;
static size_t input_count = 0;
static size_t input_position = 0;

void CHECK(bool condition, const char *mesage) {
    if (!condition) {
        printf("[CHYBA] %s\n", mesage);
        failures++;
    }
}

Token create_test_token(TokenType type) {
    Token token = {0};
    token.type = type;
    return token;
}

void set_token_stream(const Token *tokens, size_t count) {
    input_tokens = tokens;
    input_count = count;
    input_position = 0;

    current_token = get_token_buffered();
}

Token get_token_buffered(void) {

    if (input_position < input_count) {
        return input_tokens[input_position++];
    }

    return create_test_token(TOKEN_EOF);
}

// freeing AST nodes recursively
void free_ast(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->extra);
    free_ast(node->next);

    free(node);
}

void test_expr_stack_init() {

    printf("\nTest 1: expr_stack_init()\n");

    PrecStack stack;

    expr_stack_init(&stack);

    CHECK(stack.top == NULL,
          "Zasobnik nebol inicializovany na NULL");

    if (stack.top == NULL) {
        printf("[OK] expr_stack_init() uspesny!\n");
    }
}

void test_expr_stack_push() {

    printf("\nTest 2: expr_stack_push()\n");

    PrecStack stack;
    expr_stack_init(&stack);

    Token token = create_test_token(TOKEN_INTEGER);

    ASTNode dummy_node = {0};

    bool result = expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_OPERAND,
        token,
        &dummy_node
    );

    CHECK(result == true,
          "expr_stack_push() nevratil true");

    CHECK(stack.top != NULL,
          "Do zasobnika nebol vlozen prvok");

    if (stack.top != NULL) {

        CHECK(stack.top->type == SYM_TERMINAL,
              "Nespravny typ prvku");

        CHECK(stack.top->prec_index == PREC_OPERAND,
              "Nespravny prec_index");

        CHECK(stack.top->token.type == TOKEN_INTEGER,
              "Token nebol spravne ulozeny");

        CHECK(stack.top->ast_node == &dummy_node,
              "AST uzol nebol spravne ulozeny");

        if (stack.top->type == SYM_TERMINAL &&
            stack.top->prec_index == PREC_OPERAND &&
            stack.top->token.type == TOKEN_INTEGER &&
            stack.top->ast_node == &dummy_node) {

            printf("[OK] expr_stack_push() spravne vlozil prvok!\n");
        }
    }

    expr_stack_free(&stack);
}

void test_expr_stack_insert_mark() {

    printf("\nTest 3: expr_stack_insert_mark()\n");

    PrecStack stack;
    expr_stack_init(&stack);

    Token token = create_test_token(TOKEN_INTEGER);

    // terminal on top of the stack
    expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_OPERAND,
        token,
        NULL
    );

    bool result = expr_stack_insert_mark(&stack);

    CHECK(result == true,
          "Vlozenie mark nebolo uspesne");

    CHECK(stack.top != NULL && stack.top->type == SYM_MARK,
          "Mark sa nevlozil na vrch zasobnika");

    CHECK(stack.top != NULL &&
          stack.top->next != NULL &&
          stack.top->next->type == SYM_TERMINAL,
          "Terminal nie je za mark");

    expr_stack_free(&stack);

    // non-terminal on top of the stack
    expr_stack_init(&stack);

    expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_OPERAND,
        token,
        NULL
    );

    expr_stack_push(
        &stack,
        SYM_NONTERM,
        PREC_OPERAND,
        token,
        NULL
    );

    result = expr_stack_insert_mark(&stack);

    CHECK(result == true,
          "Vlozenie mark pri non-terminale zlyhalo");

    CHECK(stack.top != NULL &&
          stack.top->type == SYM_NONTERM,
          "Non-terminal sa nespravne presunul");

    CHECK(stack.top != NULL &&
          stack.top->next != NULL &&
          stack.top->next->type == SYM_MARK,
          "Mark sa nevlozil za non-terminal");

    expr_stack_free(&stack);

    if (failures == 0) {
        printf("[OK] expr_stack_insert_mark() uspesny!\n");
    }
}

void test_expr_stack_pop() {

    printf("\nTest 4: expr_stack_pop()\n");

    PrecStack stack;
    expr_stack_init(&stack);

    Token token = create_test_token(TOKEN_INTEGER);

    expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_OPERAND,
        token,
        NULL
    );

    expr_stack_push(
        &stack,
        SYM_NONTERM,
        PREC_OPERAND,
        token,
        NULL
    );

    // removal of the top element
    expr_stack_pop(&stack);

    CHECK(stack.top != NULL,
          "Po pop() mal ostat jeden prvok");

    CHECK(stack.top != NULL &&
          stack.top->type == SYM_TERMINAL,
          "Po pop() sa neodstranil vrchny prvok");

    // removal of the last element
    expr_stack_pop(&stack);

    CHECK(stack.top == NULL,
          "Zasobnik mal byt po pop() prazdny");

    // test pop() on empty stack
    expr_stack_pop(&stack);

    CHECK(stack.top == NULL,
          "Pop() na prazdnom zasobniku zmenil jeho obsah");

    if (stack.top == NULL) {
        printf("[OK] expr_stack_pop() uspesny!\n");
    }
}

void test_expr_stack_top_terminal() {

    printf("\nTest 5: expr_stack_top_terminal()\n");

    PrecStack stack;
    expr_stack_init(&stack);

    Token token = create_test_token(TOKEN_INTEGER);

    // adding a terminal to the stack
    expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_OPERAND,
        token,
        NULL
    );

    StackItem *expected_terminal = stack.top;

    // additional non-terminal on top of the terminal
    expr_stack_push(
        &stack,
        SYM_NONTERM,
        PREC_OPERAND,
        token,
        NULL
    );

    StackItem *found_terminal = expr_stack_top_terminal(&stack);

    CHECK(found_terminal != NULL,
          "Nebolo najdene ziadne terminalne slovo");

    CHECK(found_terminal == expected_terminal,
          "Funkcia vratila nespravny terminal");

    if (found_terminal == expected_terminal) {
        printf("[OK] expr_stack_top_terminal() uspesny!\n");
    }

    expr_stack_free(&stack);

    // test freeing an empty stack
    found_terminal = expr_stack_top_terminal(&stack);

    CHECK(found_terminal == NULL,
          "Pri prazdnom zasobniku mal byt vysledok NULL");
}

void test_expr_stack_free() {

    printf("\nTest 6: expr_stack_free()\n");

    PrecStack stack;
    expr_stack_init(&stack);

    Token token = create_test_token(TOKEN_INTEGER);

    // adding multiple items to the stack
    expr_stack_push(&stack, SYM_TERMINAL,
                    PREC_OPERAND, token, NULL);

    expr_stack_push(&stack, SYM_NONTERM,
                    PREC_OPERAND, token, NULL);

    expr_stack_push(&stack, SYM_MARK,
                    PREC_DOLLAR, token, NULL);

    CHECK(stack.top != NULL,
          "Zasobnik mal obsahovat prvky");

    expr_stack_free(&stack);

    CHECK(stack.top == NULL,
          "expr_stack_free() neuvolnil cely zasobnik");

    if (stack.top == NULL) {
        printf("[OK] expr_stack_free() uspesny!\n");
    }

    // test freeing an empty stack
    expr_stack_free(&stack);

    CHECK(stack.top == NULL,
          "Uvolnenie prazdneho zasobnika zmenilo jeho obsah");
}

void test_get_prec_index() {

    printf("\nTest 7: get_prec_index()\n");

    PrecStack stack;
    expr_stack_init(&stack);

    // test matematical operators
    CHECK(get_prec_index(TOKEN_MUL, &stack) == PREC_MUL_DIV,
          "TOKEN_MUL ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_DIV, &stack) == PREC_MUL_DIV,
          "TOKEN_DIV ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_PLUS, &stack) == PREC_UNARY,
          "TOKEN_PLUS mal byt unarny operator");

    CHECK(get_prec_index(TOKEN_MINUS, &stack) == PREC_UNARY,
          "TOKEN_MINUS mal byt unarny operator");


    // test relational operators
    CHECK(get_prec_index(TOKEN_LESS, &stack) == PREC_REL,
          "TOKEN_LESS ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_LESS_EQ, &stack) == PREC_REL,
          "TOKEN_LESS_EQ ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_GREATER, &stack) == PREC_REL,
          "TOKEN_GREATER ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_GREATER_EQ, &stack) == PREC_REL,
          "TOKEN_GREATER_EQ ma nespravny precedence index");


    // test equality operators
    CHECK(get_prec_index(TOKEN_EQ, &stack) == PREC_EQ_NOT_EQ,
          "TOKEN_EQ ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_NOT_EQ, &stack) == PREC_EQ_NOT_EQ,
          "TOKEN_NOT_EQ ma nespravny precedence index");


    // test parentheses
    CHECK(get_prec_index(TOKEN_LPAREN, &stack) == PREC_LPAREN,
          "TOKEN_LPAREN ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_RPAREN, &stack) == PREC_RPAREN,
          "TOKEN_RPAREN ma nespravny precedence index");


    // test operands
    CHECK(get_prec_index(TOKEN_IDENTIFIER, &stack) == PREC_OPERAND,
          "TOKEN_IDENTIFIER ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_INTEGER, &stack) == PREC_OPERAND,
          "TOKEN_INTEGER ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_DOUBLE, &stack) == PREC_OPERAND,
          "TOKEN_DOUBLE ma nespravny precedence index");

    CHECK(get_prec_index(TOKEN_STRING, &stack) == PREC_OPERAND,
          "TOKEN_STRING ma nespravny precedence index");


    // test binary plus and minus
    Token token = create_test_token(TOKEN_INTEGER);

    expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_OPERAND,
        token,
        NULL
    );

    CHECK(get_prec_index(TOKEN_PLUS, &stack) == PREC_ADD_SUB,
          "TOKEN_PLUS mal byt binarny operator");

    CHECK(get_prec_index(TOKEN_MINUS, &stack) == PREC_ADD_SUB,
          "TOKEN_MINUS mal byt binarny operator");

    expr_stack_pop(&stack);


    // test binary plus and minus after right parenthesis
    expr_stack_push(
        &stack,
        SYM_TERMINAL,
        PREC_RPAREN,
        create_test_token(TOKEN_RPAREN),
        NULL
    );

    CHECK(get_prec_index(TOKEN_PLUS, &stack) == PREC_ADD_SUB,
          "TOKEN_PLUS po pravej zatvorke mal byt binarny");

    CHECK(get_prec_index(TOKEN_MINUS, &stack) == PREC_ADD_SUB,
          "TOKEN_MINUS po pravej zatvorke mal byt binarny");

    expr_stack_free(&stack);

    // test end of expression
    CHECK(get_prec_index(TOKEN_EOF, &stack) == PREC_DOLLAR,
          "TOKEN_EOF ma nespravny precedence index");

    if (failures == 0) {
        printf("[OK] get_prec_index() uspesny!\n");
    }
}

void test_parse_single_operand(TokenType operand_type,
                               const char *operand_name) {

    printf("\nTest 8: parse_expression() - %s\n", operand_name);

    Token tokens[] = {
        create_test_token(operand_type),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 2);

    ASTNode *result = parse_expression();

    CHECK(result != NULL,
          "Jednoduchy vyraz vratil NULL");

    if (result != NULL) {

        CHECK(result->type == AST_NODE_EXPR,
              "Vysledok nema typ AST_NODE_EXPR");

        CHECK(result->val.type == operand_type,
              "Vysledny uzol obsahuje nespravny token");

        CHECK(result->left == NULL,
              "Jednoduchy operand nema mat laveho potomka");

        CHECK(result->right == NULL,
              "Jednoduchy operand nema mat praveho potomka");

        CHECK(current_token.type == TOKEN_EOF,
              "Parser nespravným sposobom spracoval koniec vyrazu");

        free_ast(result);
    }

    if (failures == 0) {
        printf("[OK] Jednoduchy vyraz %s uspesny!\n", operand_name);
    }
}

void test_parse_unary_minus() {

    printf("\nTest 9: parse_expression() - unarny minus\n");

    Token tokens[] = {
        create_test_token(TOKEN_MINUS),
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 3);

    ASTNode *result = parse_expression();

    CHECK(result != NULL,
          "Unarny minus vratil NULL");

    if (result != NULL) {

        CHECK(result->type == AST_NODE_EXPR,
              "Nespravny typ korenoveho uzla");

        CHECK(result->val.type == TOKEN_MINUS,
              "Korenovy uzol neobsahuje unarny minus");

        CHECK(result->left != NULL,
              "Unarny minus nema laveho potomka");

        if (result->left != NULL) {
            CHECK(result->left->val.type == TOKEN_INTEGER,
                  "Operand unarneho minus je nespravny");
        }

        CHECK(result->right == NULL,
              "Unarny minus nema mat praveho potomka");

        free_ast(result);
    }

    if (failures == 0) {
        printf("[OK] Unarny minus uspesny!\n");
    }
}

void test_parse_binary_add() {

    printf("\nTest 10: parse_expression() - binarne scitanie\n");

    Token tokens[] = {
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_PLUS),
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 4);

    ASTNode *result = parse_expression();

    CHECK(result != NULL,
          "Binarne scitanie vratilo NULL");

    if (result != NULL) {

        CHECK(result->type == AST_NODE_EXPR,
              "Vysledok nema typ AST_NODE_EXPR");

        CHECK(result->val.type == TOKEN_PLUS,
              "Korenovy uzol nema operator PLUS");

        CHECK(result->left != NULL,
              "Scitanie nema laveho potomka");

        CHECK(result->right != NULL,
              "Scitanie nema praveho potomka");

        if (result->left != NULL) {
            CHECK(result->left->val.type == TOKEN_INTEGER,
                  "Lavy operand nie je INTEGER");
        }

        if (result->right != NULL) {
            CHECK(result->right->val.type == TOKEN_INTEGER,
                  "Pravy operand nie je INTEGER");
        }

        free_ast(result);
    }

    if (failures == 0) {
        printf("[OK] Binarne scitanie uspesne!\n");
    }
}

void test_parse_parentheses() {

    printf("\nTest 11: parse_expression() - zatvorky\n");

    Token tokens[] = {
        create_test_token(TOKEN_LPAREN),
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_RPAREN),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 4);

    ASTNode *result = parse_expression();

    CHECK(result != NULL,
          "Vyraz v zatvorkach vratil NULL");

    if (result != NULL) {

        CHECK(result->type == AST_NODE_EXPR,
              "Vysledok nema typ AST_NODE_EXPR");

        CHECK(result->val.type == TOKEN_INTEGER,
              "Zatvorky nespravne zmenili obsah vyrazu");

        CHECK(result->left == NULL,
              "Jednoduchy vyraz v zatvorkach ma nespravneho potomka");

        free_ast(result);
    }

    if (failures == 0) {
        printf("[OK] Zatvorky uspesne!\n");
    }
}

void test_parse_empty_expression() {

    printf("\nTest 12: parse_expression() - prazdny vyraz\n");

    Token tokens[] = {
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 1);

    ASTNode *result = parse_expression();

    CHECK(result == NULL,
          "Prazdny vyraz mal vratit NULL");

    if (result == NULL) {
        printf("[OK] Prazdny vyraz spravne vratil NULL!\n");
    }
}

void test_parse_lexical_error() {

    printf("\nTest 13: parse_expression() - lexical error\n");

    Token tokens[] = {
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 2);

    ASTNode *result = parse_expression();

    CHECK(result == NULL,
          "Pri lexical error mal parser vratit NULL");

    if (result == NULL) {
        printf("[OK] Lexical error spravne vratil NULL!\n");
    }
}

void test_parse_incomplete_expression() {

    printf("\nTest 14: parse_expression() - nekompletny vyraz\n");

    Token tokens[] = {
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_PLUS),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, 3);

    ASTNode *result = parse_expression();

    CHECK(result == NULL,
          "Nekompletny vyraz nemal vytvorit AST");

    if (result != NULL) {
        free_ast(result);
    }

    if (result == NULL) {
        printf("[OK] Nekompletny vyraz spravne vratil NULL!\n");
    }
}


void test_expression_case(
    const char *name,
    const TokenType token_types[],
    size_t count,
    bool should_succeed
) {
    printf("\nTest: %s\n", name);

    int failures_before = failures;

    Token tokens[32];

    if (count >= 31) {
        printf("[CHYBA] Prilis vela tokenov\n");
        failures++;
        return;
    }

    for (size_t i = 0; i < count; i++) {
        tokens[i] = create_test_token(token_types[i]);
    }

    tokens[count] = create_test_token(TOKEN_EOF);

    set_token_stream(tokens, count + 1);

    ASTNode *result = parse_expression();

    if (should_succeed) {
        CHECK(result != NULL,
              "Parser mal uspesne spracovat vyraz");
    }
    else {
        CHECK(result == NULL,
              "Parser mal odmietnut neplatny vyraz");
    }

    free_ast(result);

    if (failures == failures_before) {
        printf("[OK] Test uspesny!\n");
    }
}


void test_new_expressions() {

    printf("\n========== NOVE TESTY VYRAZOV ==========\n");

    // 1 * 2
    TokenType expr1[] = {
        TOKEN_INTEGER,
        TOKEN_MUL,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Binarne nasobenie: 1 * 2",
        expr1, 3, true
    );


    // 1 - 2
    TokenType expr2[] = {
        TOKEN_INTEGER,
        TOKEN_MINUS,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Binarne odcitanie: 1 - 2",
        expr2, 3, true
    );


    // 1 / 2
    TokenType expr3[] = {
        TOKEN_INTEGER,
        TOKEN_DIV,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Binarne delenie: 1 / 2",
        expr3, 3, true
    );


    // 1 + 2 * 3
    TokenType expr4[] = {
        TOKEN_INTEGER,
        TOKEN_PLUS,
        TOKEN_INTEGER,
        TOKEN_MUL,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Priorita operatorov: 1 + 2 * 3",
        expr4, 5, true
    );


    // (1 + 2) * 3
    TokenType expr5[] = {
        TOKEN_LPAREN,
        TOKEN_INTEGER,
        TOKEN_PLUS,
        TOKEN_INTEGER,
        TOKEN_RPAREN,
        TOKEN_MUL,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Zatvorky: (1 + 2) * 3",
        expr5, 7, true
    );


    // 1 < 2
    TokenType expr6[] = {
        TOKEN_INTEGER,
        TOKEN_LESS,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Porovnavanie: 1 < 2",
        expr6, 3, true
    );


    // 1 == 2
    TokenType expr7[] = {
        TOKEN_INTEGER,
        TOKEN_EQ,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Rovnost: 1 == 2",
        expr7, 3, true
    );


    // +1
    TokenType expr8[] = {
        TOKEN_PLUS,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Unarny plus: +1",
        expr8, 2, true
    );


    // 1 + * 2
    TokenType expr9[] = {
        TOKEN_INTEGER,
        TOKEN_PLUS,
        TOKEN_MUL,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Neplatny vyraz: 1 + * 2",
        expr9, 4, false
    );


    // (1 + 2
    TokenType expr10[] = {
        TOKEN_LPAREN,
        TOKEN_INTEGER,
        TOKEN_PLUS,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Chybajuca zatvorka: (1 + 2",
        expr10, 4, false
    );


    // 1 + 2)
    TokenType expr11[] = {
        TOKEN_INTEGER,
        TOKEN_PLUS,
        TOKEN_INTEGER,
        TOKEN_RPAREN
    };

    test_expression_case(
        "Nadbytocna zatvorka: 1 + 2)",
        expr11, 4, false
    );


    // *1
    TokenType expr12[] = {
        TOKEN_MUL,
        TOKEN_INTEGER
    };

    test_expression_case(
        "Neplatny zaciatok: *1",
        expr12, 2, false
    );
}


void test_parentheses_precedence_ast() {

    printf("\nTest: Kontrola AST pre (1 + 2) * 3\n");

    int failures_before = failures;

    Token tokens[] = {
        create_test_token(TOKEN_LPAREN),
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_PLUS),
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_RPAREN),
        create_test_token(TOKEN_MUL),
        create_test_token(TOKEN_INTEGER),
        create_test_token(TOKEN_EOF)
    };

    set_token_stream(tokens, sizeof(tokens) / sizeof(tokens[0]));

    ASTNode *result = parse_expression();

    CHECK(result != NULL,
          "Parser nevytvoril AST");

    if (result != NULL) {

        CHECK(result->type == AST_NODE_EXPR,
              "Korenovy uzol nema typ AST_NODE_EXPR");

        CHECK(result->val.type == TOKEN_MUL,
              "Korenovy operator mal byt MUL");

        CHECK(result->left != NULL,
              "Korenovy uzol nema laveho potomka");

        if (result->left != NULL) {

            CHECK(result->left->type == AST_NODE_EXPR,
                  "Lavy potomok nema typ AST_NODE_EXPR");

            CHECK(result->left->val.type == TOKEN_PLUS,
                  "Lavy potomok mal obsahovat operator PLUS");

            CHECK(result->left->left != NULL,
                  "Scitanie nema prvy operand");

            if (result->left->left != NULL) {
                CHECK(result->left->left->val.type == TOKEN_INTEGER,
                      "Prvy operand scitania nie je INTEGER");
            }

            CHECK(result->left->right != NULL,
                  "Scitanie nema druhy operand");

            if (result->left->right != NULL) {
                CHECK(result->left->right->val.type == TOKEN_INTEGER,
                      "Druhy operand scitania nie je INTEGER");
            }
        }

        CHECK(result->right != NULL,
              "Nasobenie nema pravy operand");

        if (result->right != NULL) {
            CHECK(result->right->type == AST_NODE_EXPR,
                  "Pravy potomok nema typ AST_NODE_EXPR");

            CHECK(result->right->val.type == TOKEN_INTEGER,
                  "Pravy operand nasobenia nie je INTEGER");
        }

        CHECK(current_token.type == TOKEN_EOF,
              "Parser nespracoval cely vstupny vyraz");

        free_ast(result);
    }

    if (failures == failures_before) {
        printf("[OK] AST pre (1 + 2) * 3 je spravne!\n");
    }
}


int main() {

    printf("--- Spustam testy pre expression.c ---\n");

    test_expr_stack_init();

    test_expr_stack_push();

    test_expr_stack_insert_mark();

    test_expr_stack_pop();

    test_expr_stack_top_terminal();

    test_expr_stack_free();

    test_get_prec_index();

    // tests parse_expression()
    test_parse_single_operand(TOKEN_INTEGER, "integer");

    test_parse_single_operand(TOKEN_DOUBLE, "double");

    test_parse_single_operand(TOKEN_STRING, "string");

    test_parse_single_operand(TOKEN_IDENTIFIER, "identifier");

    test_parse_unary_minus();

    test_parse_binary_add();

    test_parse_parentheses();

    test_parse_empty_expression();

    test_parse_lexical_error();

    test_parse_incomplete_expression();

    test_new_expressions();

    test_parentheses_precedence_ast();

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
