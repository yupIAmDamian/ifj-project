#include "codegen.h"

char* current_func_name = NULL;
static int label_counter = 0;


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

            //TODO: part time fix
            gen_pops("tmp_var");
            gen_move(gen_string_from_node(node->left));
            gen_var("tmp_var");
            gen_new_line();
            break;
        case AST_NODE_RETURN:
            equation_traversal(node->left, "temp_return");
            printf("PUSHS %s\n", "temp_return");
            gen_return();
            break;
        case AST_NODE_ASSIGN:
            equation_traversal(node->right, gen_string_from_node(node->left));
            gen_new_line();
            break;
        default:
            break;
    }
}

/*
function declaration
*/
void gen_fun_def_start(){
    gen_label(current_func_name);
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

//TODO why is this needed?
/**
 * Recursively generates variable declarations for a function at the start of function.
 * @param node The current AST node.
 * @param var_counter A pointer to the variable counter.
 */
void gen_fun_def_vars_recursive(ASTNode *node){
    if (node == NULL) {
        return;
    }

    if (node->type == AST_NODE_VAR_DECL) {
        gen_defvar();
        gen_var(gen_string_from_node(node->left));
        gen_new_line();
    }

    gen_fun_def_vars_recursive(node->left);
    gen_fun_def_vars_recursive(node->right);
    gen_fun_def_vars_recursive(node->extra);
    gen_fun_def_vars_recursive(node->next);
}

void gen_fun_def_body(ASTNode *node){
    gen_code(node);
}

void gen_fun_def_end(){
    gen_popframe();
    gen_return();
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

    printf("PUSHS bool@true\n");
    gen_jumpifeqs(else_label);

    gen_code(then_body);

    gen_jump(end_label);
    
    if (else_body != NULL) {
        gen_code(else_body);
    }

    gen_label(end_label);


    free(else_label);
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
        case TOKEN_GREATER:
            gen_GTs();
            break;
        case TOKEN_GREATER_EQ:
            gen_GTs();
            gen_pushs(op1);
            gen_pushs(op2);
            gen_EQs();
            gen_ORs();
            break;
        case TOKEN_LESS:
            gen_LTs();
            break;
        case TOKEN_LESS_EQ:
            gen_LTs();
            gen_pushs(op1);
            gen_pushs(op2);
            gen_EQs();
            gen_ORs();
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
}

//While statement
void gen_while(ASTNode *node){

    ASTNode *condition_node = node->left; 
    ASTNode *body_node = node->right; 

    int label_number = label_counter++;
    char *start_label = create_label(label_number, "while", "start");
    char *end_label = create_label(label_number, "while", "end");

    gen_label(start_label);
    decide_condition(condition_node);

    printf("PUSHS bool@true\n");
    gen_jumpifeqs(end_label);

    gen_code(body_node);
    
    gen_jump(start_label);

    gen_label(end_label);

    free(start_label);
    free(end_label);
}


void gen_pushs(ASTNode *node){
    printf("PUSHS ");
    gen_argument(node);
    gen_new_line();
}

void gen_argument(ASTNode *arg_node){
    Token *token = get_token_from_node(arg_node);
    if (token->type == TOKEN_IDENTIFIER) {
        gen_var(gen_string_from_node(arg_node));
    } else {
        gen_data_type(arg_node);
    }
}

//arithmetic operations
//for every arithmetic operation we will have to push the operands onto the stack and then perform the operation
//after that will pop the result from the stack and store it in a variable
//TODO: check if the logic is correct
void equation_traversal(ASTNode *node, char *result_var){
    if (node == NULL) {
        return;
    }


    if (node->type == AST_NODE_CALL) {
        gen_fun_call(node, result_var);
        return;
    }

    equation_traversal(node->left, result_var);
    equation_traversal(node->right, result_var);

    switch (node->val.type) {
            case TOKEN_PLUS:
                gen_adds();
                gen_pops(result_var);
                break;
            case TOKEN_MINUS:
                gen_subs();
                gen_pops(result_var);
                break;
            case TOKEN_MUL:
                gen_muls();
                gen_pops(result_var);
                break;
            case TOKEN_DIV:
                gen_divs();
                gen_pops(result_var);
                break;
            default:
                gen_pushs(node);
                break;
    }
}

void gen_call(ASTNode *node, char *result_var) {
    char *label = gen_string_from_node(node);
    if (label == NULL) {
        return;
    }

    if (strcmp(label, "print") == 0) {
        printf("CALL $print\n");
    }
    else if (strcmp(label, "readLine") == 0) {
        gen_read(result_var, "string");
    }
    else if (strcmp(label, "readInt") == 0) {
        gen_read(result_var, "int");
    }
    else if (strcmp(label, "readDouble") == 0) {
        gen_read(result_var, "float");
    }
    else if (strcmp(label, "strLength") == 0) {
        gen_strlen(result_var, gen_string_from_node(node->right));
    }
    else if (strcmp(label, "toInt") == 0) {
        gen_dprint(gen_string_from_node(node->right));
        gen_float2ints();
        gen_pops(result_var);
    }
    else if (strcmp(label, "toStr") == 0) {
        gen_pushs(node->right);
        gen_int2strs();
        gen_pops(result_var);
    }
    else if (strcmp(label, "subStr") == 0) {
        gen_builtin_substr();
        gen_pops(result_var);
    }
    //handle this
    else if (strcmp(label, "ord") == 0) {
        gen_builtin_ord();
        gen_pops(result_var);
    }
    else if (strcmp(label, "chr") == 0) {
        gen_pushs(node->right);
        gen_int2chars();
        gen_pops(result_var);
    }
    else {
        printf("CALL %s\n", label);

    }
}


void gen_fun_call(ASTNode *node, char *result_var){
    ASTNode *arg_node = node->right; 
    //finding last argument in the list of arguments
    //first it will send leftmost data so function call will be in the correct order
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

    gen_call(node, result_var);
}



