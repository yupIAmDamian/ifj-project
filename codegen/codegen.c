#include "codegen.h"

char* current_func_name = NULL;
static int label_counter = 0;
//GloBal TODO
//add space after every instruction
//add newlines where needed

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
            break;
        case AST_NODE_FUNCTION:
            char *fun_name_token = string_from_token(node->left);
            current_func_name = string_from_token(fun_name_token);
            gen_fun_def_start();
            gen_fun_def_args(node->extra);
            gen_fun_def_vars(node->right);
            break;
        case AST_NODE_IF:
            gen_if(node);
            break;
        case AST_NODE_WHILE:
            gen_while(node);
            break;
        case AST_NODE_CALL:
            gen_fun_call(node, "resultvar");
            break;
        case AST_NODE_VAR_DECL:
            //TODO fix this when the variable declaration will be added to the AST
            gen_var("LF", string_from_token(node->left), 0);
            break;
        case AST_NODE_RETURN:
            gen_return();
            break;
        case AST_NODE_ADD:
        case AST_NODE_SUB:
        case AST_NODE_MUL:
        case AST_NODE_DIV:
            equation_traversal(node);
            break;
        default:
            break;
    }

}

//TODO push result of function call onto the stack and then pop it into the variable that will hold the result of the function call
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
        char *param_name = string_from_token(node);
        gen_params_declaration("LF",  current_func_name, param_name);
        node = node->next;
    }
}

//TODO why is this needed?
/**
 * Recursively generates variable declarations for a function at the start of function.
 * @param node The current AST node.
 * @param var_counter A pointer to the variable counter.
 */
void gen_fun_def_vars_recursive(ASTNode *node, int *var_counter){
    if (node == NULL) {
        return;
    }

    if (node->type == AST_NODE_VAR_DECL) {
        gen_defvar();
        gen_var("LF", string_from_token(node->left), *var_counter);
        gen_new_line();
        (*var_counter)++;
    }

    gen_fun_def_vars_recursive(node->left, var_counter);
    gen_fun_def_vars_recursive(node->right, var_counter);
    gen_fun_def_vars_recursive(node->extra, var_counter);
    gen_fun_def_vars_recursive(node->next, var_counter);
}

void gen_fun_def_vars(ASTNode *node){
    int var_counter = 0;
    gen_fun_def_vars_recursive(node, &var_counter);
}

void gen_fun_def_body(ASTNode *node){
    gen_code(node);
}

void gen_fun_def_end(){
    gen_popframe();
    gen_return();
}

/*
function call
*/
void gen_fun_call(ASTNode *node, char *result_var){
    ASTNode *arg_node = node->right; 
    int arg_count = 0;
    //finding last argument in the list of arguments
    //first it will send leftmost data so function call will be in the correct order
    while(arg_node != NULL){
        arg_count++;
        if(arg_node->next != NULL) {
            arg_node = arg_node->next;
        } else {
            break;
        }
    }

    while(arg_node != NULL){
        Token token = get_token_from_node(arg_node);
        //TODO check if logic is correct
        //TODO- this type of code repeats itself in many places, maybe we can create a function for it
        if(token.type == TOKEN_IDENTIFIER){
            gen_pushs(string_from_token(arg_node));
        } else {
            gen_pushs_data_types(&token);
        }
        gen_new_line();

        if(arg_node->prev != NULL) {
            arg_node = arg_node->prev;//prev will be added to the ASTNode struct
        } else {
            break;
        }
    }

    gen_call(string_from_token(node->left));

    gen_pops(result_var);

    //take care of the arguments that were pushed onto the stack
    for(int i = 0; i < arg_count; i++){
        gen_pops();
    }
}

//If statement
void gen_if(ASTNode *node){
    // Assuming node->left is the condition, node->right is the then_body, and node->extra is the else_body
    ASTNode *condition_node = node->left;
    ASTNode *then_body = node->right;
    ASTNode *else_body = node->extra;

    //create labels according to name mengling
    int label_number = label_counter++;
    char *else_label = create_label(label_number, "if", "else");
    char *end_label = create_label(label_number, "if", "end");


    decide_condition(condition_node);

    gen_pushs("bool@false");
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
    Token *condition = get_token_attributes(condition_node); 

    ASTNode *op1 = condition_node->left;
    Token *op1_attr = get_token_attributes(op1);

    ASTNode *op2 = condition_node->right;
    Token *op2_attr = get_token_attributes(op2);

    gen_pushs(op1_attr->attribute.string_value);
    gen_pushs(op2_attr->attribute.string_value);

    switch(condition->type){
        case TOKEN_GREATER:
            gen_GTs();
            break;
        case TOKEN_GREATER_EQ:
            gen_GTs();
            gen_pushs(op1_attr->attribute.string_value);
            gen_pushs(op2_attr->attribute.string_value);
            gen_EQs();
            gen_ORs();
            break;
        case TOKEN_LESS:
            gen_LTs();
            break;
        case TOKEN_LESS_EQ:
            gen_LTs();
            gen_pushs(op1_attr->attribute.string_value);
            gen_pushs(op2_attr->attribute.string_value);
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

    gen_pushs("bool@false");
    gen_jumpifeqs(end_label);

    gen_code(body_node);
    gen_jump(start_label);

    gen_label(end_label);

    free(start_label);
    free(end_label);
}

//generation of constants
//Consider: types NIL, BOOL, ARRAY
void gen_data_type(ASTNode *node){
    Token token = get_token_from_node(node);
    switch (token.type) {
        case TOKEN_INTEGER:
            printf("int@%d", token.attribute.integer_value);
            break;
        //TODO chceck if float is correct as a
        case TOKEN_DOUBLE:
            printf("float@%a", token.attribute.double_value);
            break;
        case TOKEN_STRING:
            printf("string@%s", token.attribute.string_value);
            break;
        default:
            break;
    }
}

//pushs 
//push on stack func
//TODO resolve this
void gen_pushs(char* symb){
    printf("PUSHS");
    printf("%s", symb);
    gen_new_line();
}

void gen_pushs_data_types(Token *symb){
    printf("PUSHS");
    gen_data_type(symb);
    gen_new_line();
}

//arithmetic operations
//for every arithmetic operation we will have to push the operands onto the stack and then perform the operation
//after that will pop the result from the stack and store it in a variable
//TODO: check if the logic is correct
void equation_traversal(ASTNode *node){
    if(node == NULL) return;

    equation_traversal(node->left);
    equation_traversal(node->right);

    if(node->type == AST_NODE_ADD || node->type == AST_NODE_SUB || node->type == AST_NODE_MUL || node->type == AST_NODE_DIV){
        arithmetic_operation(node);
    } else {
        //TODO push only one thing
        if(node->val.type == TOKEN_IDENTIFIER){
            gen_pushs(string_from_token(node));
        } else {
            gen_pushs_data_types(&node->val);
        }
    }
}

void arithmetic_operation(ASTNode *node){
    switch(node->type){
        case AST_NODE_ADD:
            gen_adds();
            break;
        case AST_NODE_SUB:
            gen_subs();
            break;
        case AST_NODE_MUL:
            gen_muls();
            break;
        case AST_NODE_DIV:
            gen_divs();
            break;
        default:
            break;
    }
}