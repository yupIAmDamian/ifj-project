// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#include "codegen.h"

char* current_func_name = NULL;
int label_counter = 0;

void gen_short_circuit(ASTNode *node, bool is_and) {
    int id = fresh_id();
    char *decided = is_and ? "bool@false" : "bool@true";

    gen_operand(node->left);           
    printf("PUSHS %s\n", decided);
    printf("JUMPIFEQS $sc$skip$%d\n", id);   

    gen_operand(node->right);
    printf("JUMP $sc$end$%d\n", id);

    printf("LABEL $sc$skip$%d\n", id);
    printf("PUSHS %s\n", decided);
    printf("LABEL $sc$end$%d\n", id);
}

void gen_operand(ASTNode *node) {
    if (node == NULL) return;
    if (node->type == AST_NODE_INT2FLOAT) {
        equation_traversal(node, NULL);
        return;
    }
    if (node->type == AST_NODE_VAR || node->type == AST_NODE_TERM){
        gen_pushs(node);
    }else{
        decide_condition(node);
    }

}

void gen_push_value(ASTNode *node) {
    if (node->type == AST_NODE_VAR || node->type == AST_NODE_TERM) {
        gen_pushs(node);
    } else if (node->type == AST_NODE_CALL) {
        gen_fun_call(node);
    } else if (node->inferred_type == TYPE_BOOL) {
        decide_condition(node);         
    } else {
        equation_traversal(node, NULL);  
    }
}

void gen_code(ASTNode *root){
    while(root != NULL){
        gen_node(root);

        root = root->next;
    }
}

void gen_node(ASTNode *node){
    switch(node->type){
        case AST_NODE_PROGRAM:
            create_head();
            if(node->uses_ord == true) {
                gen_builtin_ord();
            }

            if(node->uses_substr == true) {
                gen_builtin_substr();
            }

            gen_code(node->left);
            break;
        case AST_NODE_FUNCTION: {
            if (node->is_main) {
                current_func_name = "main";
            } else {
                current_func_name = gen_string_from_node(node);
            }
            gen_fun_def_start();
            gen_fun_def_args(node->left);
            gen_fun_def_vars_recursive(node->right);
            gen_fun_init_args(node->left);

            gen_fun_def_body(node->right);
            gen_fun_def_end();
            gen_new_line();
            break;
        }
        case AST_NODE_IF:
            gen_if(node);
            break;
        case AST_NODE_WHILE:
            gen_while(node);
            break;
        case AST_NODE_CALL:
            gen_fun_call(node);
            break;
        case AST_NODE_VAR_DECL:
        case AST_NODE_ASSIGN:
            gen_push_value(node->right);
            gen_pops(gen_string_from_node(node->left));
            break;
        case AST_NODE_RETURN:
            if (node->left != NULL) {
                gen_push_value(node->left);
            }
            gen_popframe();
            gen_return();
            gen_new_line();
            break;
        default:
            break;
    }
}


void gen_compare_operand(ASTNode *node, bool want_float) {
    gen_operand(node);

    if (want_float && node->type != AST_NODE_INT2FLOAT && node->inferred_type == TYPE_INT){
        gen_int2floats();
    }
}
//Bool operations
void decide_condition(ASTNode *condition_node){
    if (condition_node == NULL)
        return;

    if (condition_node->type == AST_NODE_CALL) {
        gen_fun_call(condition_node);
        return;
    }

    if (condition_node->type == AST_NODE_VAR || condition_node->type == AST_NODE_TERM) {
        gen_pushs(condition_node);
        return;
    }

    Token *condition = get_token_from_node(condition_node);
    if (condition == NULL) return;

    if (condition->type == TOKEN_AND || condition->type == TOKEN_OR) {
        gen_short_circuit(condition_node, condition->type == TOKEN_AND);
        return;
    }

    switch (condition->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MUL:
        case TOKEN_DIV:
            equation_traversal(condition_node, NULL);  
            return;
        case TOKEN_TRUE:
            printf("PUSHS bool@true\n");
            return;
        case TOKEN_FALSE:
            printf("PUSHS bool@false\n");
            return;
        default:
            if (condition->type == TOKEN_IDENTIFIER || condition->type == TOKEN_INTEGER ||
                condition->type == TOKEN_DOUBLE || condition->type == TOKEN_STRING) {
                gen_pushs(condition_node);
            }
            break;
    }

    ASTNode *op1 = condition_node->left;
    ASTNode *op2 = condition_node->right;

    bool want_float = (op1 != NULL && is_float_operand(op1)) ||
                    (op2 != NULL && is_float_operand(op2));

    if (op1 != NULL) gen_compare_operand(op1, want_float);
    if (op2 != NULL) gen_compare_operand(op2, want_float);

    switch (condition->type) {
        case TOKEN_LESS_EQ:
            gen_GTs(); 
            gen_NOTs();
            break;
        case TOKEN_GREATER:  
            gen_GTs(); 
            break;
        case TOKEN_GREATER_EQ: 
            gen_LTs(); 
            gen_NOTs();
            break;
        case TOKEN_LESS:     
            gen_LTs(); 
            break;
        case TOKEN_EQ:       
            gen_EQs(); 
            break;
        case TOKEN_NOT_EQ:   
            gen_EQs(); 
            gen_NOTs(); 
            break;
        case TOKEN_NOT:      
            gen_NOTs(); 
            break;
        default:
            break;
    }
}

void gen_condition_statement_while_if(ASTNode *condition_node, ASTNode *then_body, char *else_label){
    decide_condition(condition_node);
    printf("PUSHS bool@false\n");
    gen_jumpifeqs(else_label);
    gen_code(then_body);
}

//If statement
void gen_if(ASTNode *node){
    ASTNode *condition_node = node->left;
    ASTNode *then_body = node->right;
    ASTNode *else_body = node->extra;

    int label_number = label_counter++;
    char *else_label = create_label(label_number, "if", "else");
    char *end_label  = create_label(label_number, "if", "end");

    gen_condition_statement_while_if(condition_node, then_body, else_label);

    if (!always_returns(then_body)) {
        gen_jump(end_label);
    }

    gen_label(else_label);            
    if (else_body != NULL) {
        gen_code(else_body);
    }

    gen_label(end_label);

    free(else_label);
    free(end_label);
}

//While statement
void gen_while(ASTNode *node){

    ASTNode *condition_node = node->left; 
    ASTNode *body_node = node->right; 

    int label_number = label_counter++;
    char *start_label = create_label(label_number, "while", "start");
    char *end_label = create_label(label_number, "while", "end");

    gen_label(start_label);

    gen_condition_statement_while_if(condition_node, body_node, end_label);
    
    gen_jump(start_label);

    gen_label(end_label);

    free(start_label);
    free(end_label);
}

void equation_traversal(ASTNode *node, char *result_var) {
    if (node == NULL) {
        return;
    }

    if (node->type == AST_NODE_CALL) {
        gen_fun_call(node);
        return;
    }

    if (node->type == AST_NODE_INT2FLOAT) {
        equation_traversal(node->left, result_var);
        gen_int2floats();
        return;
    }
    bool concat = false;
    if(node->left != NULL) {
        if (node->left->inferred_type == TYPE_STRING) {
            concat = true;
        }
    }
    if ((node->type == AST_NODE_EXPR || (node->type == AST_NODE_TERM && !concat)) &&
        node->left == NULL && node->right == NULL) {
        gen_pushs(node);
        return;
    }

    equation_traversal(node->left, result_var);
    equation_traversal(node->right, result_var);

    switch (node->val.type) {
        case TOKEN_PLUS:
            if (concat) {
                printf("POPS %s\n", concat_tmp(1));                       
                printf("POPS %s\n", concat_tmp(0)); 
                printf("CONCAT %s %s %s\n", concat_tmp(0), concat_tmp(0), concat_tmp(1));
                printf("PUSHS %s\n", concat_tmp(0));
            } else {
                gen_adds();
            }
            break;
        case TOKEN_MINUS:
            if (node->right != NULL) {
                gen_subs();
            } else {
                if (node->inferred_type == TYPE_FLOAT)
                    printf("PUSHS float@-0x1p+0\n");
                else
                    printf("PUSHS int@-1\n");
                gen_muls();
            }
            break;
        case TOKEN_MUL:
            gen_muls();
            break;
        case TOKEN_DIV:
            if (node->inferred_type == TYPE_INT)
                gen_idivs();      
            else
                gen_divs(); 
            break;
        default:
            break;
    }
}

void gen_fun_call(ASTNode *node) {
    char *label = gen_string_from_node(node);

    if (matching_string(label, "print")) {
        gen_push_value(node->right);
        printf("POPS %s\n", scratch());
        printf("WRITE %s\n", scratch());
        return;
    }

    /* Arguments from right to left*/
    ASTNode *last = node->right;
    while (last != NULL && last->next != NULL)
        last = last->next;
    for (ASTNode *arg = last; arg != NULL; arg = arg->prev)
        gen_push_value(arg);

    gen_call(node);
}

//Function Def 
void gen_fun_def_start() {
    gen_label(current_func_name);

    if (in_main()) {
        printf("DEFVAR GF@$tmp\n");
        printf("DEFVAR GF@$tmp1\n");
        printf("DEFVAR GF@$tmp2\n");
        return;
    }

    gen_createframe();
    gen_pushframe();
    printf("DEFVAR LF@$tmp\n");
    printf("DEFVAR LF@$tmp1\n");
    printf("DEFVAR LF@$tmp2\n");
}

void gen_fun_def_args(ASTNode *node){
    while(node != NULL){
        char *param_name = gen_string_from_node(node);
        gen_params_declaration(param_name);
        node = node->next;
    }
}

void gen_fun_init_args(ASTNode *node){
    while(node != NULL){
        char *param_name = gen_string_from_node(node);
        gen_pops(param_name);
        node = node->next;
    }
}

void gen_fun_def_body(ASTNode *node) {
    while (node != NULL) {
        if(node->next == NULL && node->type == AST_NODE_EXPR){
            if(in_main()) {
                equation_traversal(node, NULL);
            } else {
                gen_push_value(node);
            }
        }else {
            gen_node(node);
        }

        node = node->next;
    }
}

void gen_fun_def_end() {
    if (in_main()) {
        printf("EXIT int@0\n");
        return;
    }

    gen_popframe();
    gen_return();
}

void gen_fun_def_vars_recursive(ASTNode *node){
    if (node == NULL) {
        return;
    }

    if (node->type == AST_NODE_VAR_DECL) {
        gen_defvar();
        printf("%s", gen_string_from_node(node->left));
        gen_new_line();
    }

    gen_fun_def_vars_recursive(node->left);
    gen_fun_def_vars_recursive(node->right);
    gen_fun_def_vars_recursive(node->extra);
    gen_fun_def_vars_recursive(node->next);
}