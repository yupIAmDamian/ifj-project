// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#include "codegen.h"

char* current_func_name = NULL;
static int label_counter = 0;
static bool locked_poping = false;

char *locked_pushing_and_popping[] = {"strLength",  "readLine", "readInt", "readDouble"};

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
            gen_fun_call(node, NULL);
            break;
        case AST_NODE_VAR_DECL:
            equation_traversal(node->right, gen_string_from_node(node->left));

            if (!locked_poping) {
                gen_pops(gen_string_from_node(node->left));
            }

            break;
        case AST_NODE_RETURN:
            equation_traversal(node->left, NULL);
            gen_popframe();
            gen_return();
            gen_new_line();
            break;
        case AST_NODE_ASSIGN:
            equation_traversal(node->right, NULL);

            if (!locked_poping) {
               gen_pops(gen_string_from_node(node->left));
            }
            break;
        default:
            break;
    }
}

/*
function declaration
*/
void gen_fun_def_start() {
    gen_label(current_func_name);

    if (strcmp(current_func_name, "main") == 0) {
        return;
    }

    gen_createframe();
    gen_pushframe();
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
        if (node->next == NULL &&
            node->type == AST_NODE_EXPR &&
            strcmp(current_func_name, "main") != 0) {
            equation_traversal(node, NULL);
        } else {
            gen_node(node);
        }

        node = node->next;
    }
}
void gen_fun_def_end() {
    if (strcmp(current_func_name, "main") == 0) {
        printf("EXIT int@0\n");
        return;
    }

    gen_popframe();
    gen_return();
}

//TODO why is this needed?

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

//If statement
void gen_if(ASTNode *node){
    ASTNode *condition_node = node->left;
    ASTNode *then_body = node->right;
    ASTNode *else_body = node->extra;

    int label_number = label_counter++;
    char *else_label = create_label(label_number, "if", "else");
    char *end_label = create_label(label_number, "if", "end");

    decide_condition(condition_node);

    gen_jumpifeqs(else_label);

    gen_code(then_body);

    if (!always_returns(then_body)) {
        gen_jump(end_label);
    }
    
    if (else_body != NULL) {
        gen_label(else_label);
        gen_code(else_body);
    }

    gen_label(end_label);


    free(else_label);
    free(end_label);
}


bool always_returns(ASTNode *node) {
    if (node == NULL) {
        return false;
    }

    while (node->next != NULL) {
        node = node->next;
    }

    if (node->type == AST_NODE_RETURN) {
        return true;
    }

    if (node->type == AST_NODE_IF) {
        return node->extra != NULL &&
               always_returns(node->right) &&
               always_returns(node->extra);
    }

    return false;
}


//While statement
//TODO check what it does if there is return statement in the while loop
void gen_while(ASTNode *node){

    ASTNode *condition_node = node->left; 
    ASTNode *body_node = node->right; 

    int label_number = label_counter++;
    char *start_label = create_label(label_number, "while", "start");
    char *end_label = create_label(label_number, "while", "end");

    gen_label(start_label);
    decide_condition(condition_node);

    gen_jumpifeqs(end_label);

    gen_code(body_node);
    
    gen_jump(start_label);

    gen_label(end_label);

    free(start_label);
    free(end_label);
}

//TODO
/*
check if my logic is correct
i will push every variable onto the stack and then on top there will be the result of the condition
*/
void decide_condition(ASTNode *condition_node){
    Token *condition = get_token_from_node(condition_node); 

    ASTNode *op1 = condition_node->left;
    gen_pushs(op1);

    ASTNode *op2 = condition_node->right;
    gen_pushs(op2);

    switch(condition->type){
        case TOKEN_LESS_EQ:
        case TOKEN_GREATER:
            gen_GTs();
            break;
        case TOKEN_GREATER_EQ:
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
        default:
            break;

    }

    if(condition->type == TOKEN_LESS_EQ || condition->type == TOKEN_GREATER_EQ) {
        printf("PUSHS bool@true\n");
    }else{
        printf("PUSHS bool@false\n");
    }
}

//arithmetic operations
//for every arithmetic operation we will have to push the operands onto the stack and then perform the operation
//after that will pop the result from the stack and store it in a variable
//TODO: check if the logic is correct
void equation_traversal(ASTNode *node, char *result_var) {
    locked_poping = false;
    if (node == NULL) {
        return;
    }

    if (node->type == AST_NODE_CALL) {
        gen_fun_call(node, result_var);
        return;
    }

    if (node->type == AST_NODE_INT2FLOAT) {
        equation_traversal(node->left, result_var);
        printf("INT2FLOATS\n");
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
            if(concat) {
                printf("CONCAT ");
                gen_argument(node->left);
                printf(" ");
                gen_argument(node->left);
                printf(" ");
                gen_argument(node->right);
                gen_new_line();
                locked_poping = true;
            } else {
                gen_adds();
            }

            break;
        case TOKEN_MINUS:
            if (node->right != NULL) {
                gen_subs();
            } else {
                printf("PUSHS int@-1\n");
                gen_muls();
            }
            break;
        case TOKEN_MUL:
            gen_muls();
            break;
        case TOKEN_DIV:
            gen_divs();
            break;
        default:
            break;
    }
}


//TODO read specifications and complete it
void gen_call(ASTNode *node, char *result_var) {
    char *label = gen_string_from_node(node);
    if (label == NULL) {
        return;
    }

    else if (strcmp(label, "readLine") == 0) {
        gen_read_default(result_var,"string", "string@");
    }
    else if (strcmp(label, "readInt") == 0) {
        gen_read_default(result_var, "int", "int@0");
    }
    else if (strcmp(label, "readDouble") == 0) {
        gen_read_default(result_var, "float", "float@0x0p+0");
    }
    else if (strcmp(label, "readBoolean") == 0) {
        gen_read_default(result_var, "float", "float@0x0p+0");
    }
    else if (strcmp(label, "strLength") == 0) {
        gen_strlen();
        printf("%s ", result_var);
        gen_argument(node->right);
        gen_new_line();
    }
    else if (strcmp(label, "toInt") == 0) {
        gen_float2ints();
    }
    else if (strcmp(label, "toStr") == 0) {
        if (node->right != NULL) {
            if (node->right->inferred_type == TYPE_INT) {
                gen_int2strs();
            } else if (node->right->inferred_type == TYPE_FLOAT) {
                printf("FLOAT2STRS\n");
            }
        }
    }
    else if (strcmp(label, "chr") == 0) {
        gen_int2chars();
    }
    else {
        printf("CALL %s\n", label);
    }
}


void gen_fun_call(ASTNode *node, char *result_var){
    ASTNode *arg_node = node->right; 
    //finding last argument in the list of arguments
    //first it will send leftmost data so function call will be in the correct order
    char *label = gen_string_from_node(node);

    if (strcmp(label, "print") == 0) {
        gen_write(node->right);
        return;
    }

    //removing pushing and popping for functions that are locked
    if(check_if_string_is_not_in_array(label, locked_pushing_and_popping, sizeof(locked_pushing_and_popping) / sizeof(locked_pushing_and_popping[0]))) {
    while(arg_node != NULL){
        if(arg_node->next != NULL) {
            arg_node = arg_node->next;
        } else {
            break;
        }
    }

    while(arg_node != NULL){
        gen_pushs(arg_node);

        if(arg_node->prev != NULL) {
            arg_node = arg_node->prev;//prev will be added to the ASTNode struct
        } else {
            break;
        }
    }
    }else{
        locked_poping = true;
    }

    gen_call(node, result_var);
}
