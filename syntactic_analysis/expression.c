// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#include <stdio.h>
#include <stdlib.h>
#include "expression.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

void expr_stack_init(PrecStack *stack) {
    stack->top = NULL;
}

bool expr_stack_push(PrecStack *stack, StackItemType type, PrecTableIndex index, Token token, ASTNode *ast_node) {
    StackItem *new_item = (StackItem *)malloc(sizeof(StackItem));
    if (new_item == NULL) {
        return false;
    }
    new_item->type = type;
    new_item->prec_index = index;
    new_item->token = token;
    new_item->ast_node = ast_node;
    new_item->next = stack->top;
    stack->top = new_item;
    return true;
}

bool expr_stack_insert_mark(PrecStack *stack) {
    StackItem *new_item = (StackItem *)malloc(sizeof(StackItem));
    if (new_item == NULL) {
        return false;
    }
    new_item->type = SYM_MARK;
    new_item->ast_node = NULL;
    new_item->prec_index = PREC_DOLLAR;
    
    if (stack->top->type == SYM_TERMINAL) {
        new_item->next = stack->top;
        stack->top = new_item;
    }
    else {
        new_item->next = stack->top->next;
        stack->top->next = new_item;
    }

    return true;
}

void expr_stack_pop(PrecStack *stack) {
    if (stack->top != NULL) {
        StackItem *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp);
    }
}

StackItem *expr_stack_top_terminal(PrecStack *stack) {
    StackItem *current = stack->top;
    while (current != NULL) {
        if (current->type == SYM_TERMINAL) {
            return current;
        }
        current = current->next;
    }
    return NULL; // no terminal found
}

void expr_stack_free(PrecStack *stack) {
    while (stack->top !=NULL) {
        expr_stack_pop(stack);
    }
}

PrecTableIndex get_prec_index(TokenType token_type) {
    switch (token_type) {
        case TOKEN_MUL:
        case TOKEN_DIV:
            return PREC_MUL_DIV;
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return PREC_ADD_SUB;
        case TOKEN_LESS:
        case TOKEN_LESS_EQ:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQ:
        case TOKEN_EQ:
        case TOKEN_NOT_EQ:
            return PREC_REL;
        case TOKEN_LPAREN:
            return PREC_LPAREN;
        case TOKEN_RPAREN:
            return PREC_RPAREN;
        case TOKEN_IDENTIFIER:
        case TOKEN_INTEGER:
        case TOKEN_DOUBLE:
        case TOKEN_STRING:
            return PREC_OPERAND;
        default:
            return PREC_DOLLAR; // end or unknown token

    }
}

// precedence table
const PrecRelation precedence_table[7][7] = {
    // PREC_MUL_DIV, PREC_ADD_SUB, PREC_REL, PREC_LPAREN, PREC_RPAREN, PREC_OPERAND, PREC_DOLLAR
    { P_R, P_R, P_R, P_S, P_R, P_S, P_R }, // PREC_MUL_DIV
    { P_S, P_R, P_R, P_S, P_R, P_S, P_R }, // PREC_ADD_SUB
    { P_S, P_S, P_N, P_S, P_R, P_S, P_R }, // PREC_REL
    { P_S, P_S, P_S, P_S, P_E, P_S, P_N }, // PREC_LPAREN
    { P_R, P_R, P_R, P_N, P_R, P_N, P_R }, // PREC_RPAREN
    { P_R, P_R, P_R, P_N, P_R, P_N, P_R }, // PREC_OPERAND
    { P_S, P_S, P_S, P_S, P_N, P_S, P_N }  // PREC_DOLLAR
};

ASTNode* parse_expression() {
    PrecStack stack;
    expr_stack_init(&stack);

    Token dollar_token;
    dollar_token.type = TOKEN_EOF; 
    expr_stack_push(&stack, SYM_TERMINAL, PREC_DOLLAR, dollar_token, NULL);

    while (true) {
        StackItem *top_term = expr_stack_top_terminal(&stack);
        PrecTableIndex input_idx = get_prec_index(current_token.type);
        PrecRelation relation = precedence_table[top_term->prec_index][input_idx];

        if (relation == P_S) {
            expr_stack_insert_mark(&stack);
            expr_stack_push(&stack, SYM_TERMINAL, input_idx, current_token, NULL);
            current_token = get_next_token();
            if (current_token.type == TOKEN_ERROR) {
                expr_stack_free(&stack);
                return NULL; // lexical error
            }
        }
        else if (relation == P_R) {
            StackItem items[3];
            int count = 0;

            while (stack.top != NULL && stack.top->type != SYM_MARK) {
                if (count < 3) {
                    items[count] = *(stack.top);
                }
                count++;
                expr_stack_pop(&stack);
            }

            if (stack.top != NULL && stack.top->type == SYM_MARK) {
                expr_stack_pop(&stack);
            }

            ASTNode *new_node = NULL;
            if (count == 1) {
                new_node = create_ast_node(AST_NODE_EXPR, NULL, NULL, NULL, items[0].token);
            }

            else if (count == 3) {
                if (items[2].token.type == TOKEN_LPAREN && items[0].token.type == TOKEN_RPAREN) {
                    new_node = items[1].ast_node;
                }

                else {
                    new_node = create_ast_node(AST_NODE_EXPR, items[2].ast_node, items[0].ast_node, NULL, items[1].token);
                }
            } 

            else {
                expr_stack_free(&stack);
                return NULL; // syntax error  
            }

            Token dummy_token;
            dummy_token.type = TOKEN_EOF;
            expr_stack_push(&stack, SYM_NONTERM, PREC_OPERAND, dummy_token, new_node);
        }

        else if (relation == P_E) {
            expr_stack_push(&stack, SYM_TERMINAL, input_idx, current_token, NULL);
            current_token = get_next_token();
            if (current_token.type == TOKEN_ERROR) {
                expr_stack_free(&stack);
                return NULL; // lexical error
            }
        }
        else if (relation == P_N) {
            if (top_term->prec_index == PREC_DOLLAR && input_idx == PREC_DOLLAR) {
                ASTNode *final_tree = stack.top->ast_node;
                expr_stack_free(&stack);
                return final_tree;
            }
            else {
                expr_stack_free(&stack);
                return NULL; // syntax error
            }
        }
    }
}