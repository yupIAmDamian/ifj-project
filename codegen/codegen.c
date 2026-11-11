#include "codegen.h"

char* current_func_name = NULL;

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
            break;
        case AST_NODE_IF:
            gen_if(node);
            break;
        case AST_NODE_WHILE:
            gen_while(node);
            break;
        case AST_NODE_CALL: // there is no AST_NODE_CALL in the enum, but it will be added 
            gen_fun_call(node, "resultvar");
            break;
        case AST_NODE_VAR_DECL:
            gen_var(node);
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

void gen_fun_def_args(ASTNode *node, char *function_name ){
    while(node != NULL){
        char *param_name = string_from_token(node);
        gen_params_declaration("LF",  function_name, param_name);

        node = node->next;
    }
}

void gen_fun_def_vars(ASTNode *node, int *var_counter, char *function_name){
    /*
    TODO: go through the AST and find all variable declarations and generate code for them
    */
   while(node != NULL){
        //it needs to check also inside the function body for variable declarations, so it needs to go through the AST recursively
        
        node = node->next;
    }
}

//TODO: pass function name to the body - if its neccessary
void gen_fun_def_body(ASTNode *node, char *function_name){
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
        //there could be either 
        //TODO check if logic is correct
        if(token.type == TOKEN_IDENTIFIER){
            gen_pushs(string_from_token(arg_node));
        } else {
            gen_pushs_data_types(&token);
        }

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
    char *else_label = create_label(1, "if", "else");
    char *end_label = create_label(1, "if", "end");


    decide_condition(condition_node);

    //do i need to push the result of the condition onto the stack? i think yes, because i will need to check it with jumpifeqs
    pushs("LF@cond");


    //TODO: check this if the logic is correct, i will push every variable onto the stack and then on top there will be the result of the condition
    gen_jumpifneqs(else_label);

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

    char *start_label = create_label(1, "while", "start");
    char *end_label = create_label(1, "while", "end");

    gen_label(start_label);
    decide_condition(condition_node);

    //TODO to every ifneq or sume we need to pass to stack false or true so we can compare those results
    gen_jumpifneqs(end_label); // if could be jumpifegs if we pass false

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