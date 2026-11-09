#include "codegen.h"

void create_head(){
    printf(".IFJcode26\n");
    printf("JUMP $main\n\n");
}

Token get_token_attributes(ASTNode *node) {
    Token token;
    token.type = node->data_type;
    token.value = node->value;
    return token;
}

void gen_code(ASTNode *root){
    while(root != NULL){
        gen_node(root);

        root = root->next;
    }
}

void gen_node(ASTNode *root){
    switch(root->type){
        //chyba tu este = 
        case AST_NODE_PROGRAM:
            create_head();
            break;
        case AST_NODE_FUNCTION:
            gen_fun_def_start(root->left);
            // Generate code for function body
            gen_fun_def_args(root->extra); // Assuming extra holds the function parameters
            gen_code(root->right); 
            gen_fun_def_end();
            break;
        case AST_NODE_CALL:
            //toto domysliet - treba niecim nahradit result var
            gen_fun_call(root, "resultvar");
            break;
        case AST_NODE_VAR_DECL:
            gen_var(root);
            break;
        case AST_NODE_IF:
            gen_if(root);
            break;
        case AST_NODE_WHILE:
            gen_while(root);
            break;
        case AST_NODE_RETURN:
            gen_return(root);
            break;
        case AST_NODE_ADD:
            gen_add(root);
            break;
        case AST_NODE_SUB:
            gen_sub(root);
            break;
        case AST_NODE_MUL:
            gen_mul(root);
            break;
        case AST_NODE_DIV:
            gen_div(root);
            break;
        default:
            // Handle other node types if necessary
            break;
    }
}

void gen_fun_def_start(ASTNode *root){
    Token token = get_token_attributes(root);
    gen_label(token.value);
    gen_createframe();
    gen_pushframe();
}

void gen_fun_def_args(ASTNode *root){
    while(root != NULL){
        gen_params_declaration(root->value);
        root = root->next;
    }
}

void gen_params_declaration(char *param_name){
    gen_defvar(param_name);
    gen_pops(param_name);
}

void gen_fun_def_end(ASTNode *root){
    gen_popframe();
    gen_return();
}


void gen_fun_call(ASTNode *root, char *result_var){
    //predtymto musi byt inicializovana premenna, do ktorej sa ulozi navratova hodnota
    //cdecl convention
    //arguments
    ASTNode *arg_node = root->left; // Assuming the left child holds the arguments
    while(arg_node != NULL){
        gen_pushs(arg_node->value, arg_node->data_type);
        if(arg_node->next != NULL) {
            arg_node = arg_node->next;
        } else {
            break;
        }
    }

    //Call the function
    gen_call(root->left);

    gen_pops(variable_gen(result_var));


    //obsah funkcie sa ulozi do zasobnika, takze to musim vybrat a ulozit do nejakej premennej
    gen_pops(variable_gen(root)); // Replace "result" with the actual variable name you want to store the result in

    //vypopuje to z datoveho zasobnika a ulozi do premennej, ktora je vygenerovana z ASTNode
    while(arg_node != NULL){
        gen_pops(variable_gen(arg_node));
        arg_node = arg_node->prev;
    }
}

void gen_if(ASTNode *root){
    /*toto moze ale nemusi byt. podla toho ako si to urobim*/
    ASTNODE *condition_node = root->left; // Assuming the left child holds the condition
    Token *token_attribute = get_token_attributes(condition_node); // Assuming the left child holds the condition

    decide_condition(token_attribute->value, condition_node);

    gen_jumpifeq("$else_1", variable_gen(condition_node), "bool@false");

    gen_code(root->right);
    gen_jump("$end_if_1");

    gen_label("$else_1");
    gen_code(root->extra);


    gen_label("$end_if_1");

    """
    # 1. Vyhodnotenie podmienky (napr. x > 5)
GT LF@cond LF@x int@5

# 2. Ak je podmienka PRAVDA, skoč do ELSE vetvy (alebo naopak)
JUMPIFEQ $else_1 LF@cond bool@false

# --- THEN VETVA (vykoná sa, ak podmienka PLATÍ) ---
# ... instrukcie pre then ...

# Po dokončení THEN vetvy musíš preskočiť ELSE vetvu!
JUMP $end_if_1

# --- ELSE VETVA (vykoná sa, ak podmienka NEPLATÍ) ---
LABEL $else_1
# ... instrukcie pre else ...

# --- KONIEC IF BLOKU ---
LABEL $end_if_1

Zhrnutie instrukcií pre if:

    JUMPIFEQ label symb1 symb2: Skočí na label, ak symb1 == symb2.

    JUMPIFNEQ label symb1 symb2: Skočí na label, ak symb1 != symb2."""
}

void decide_condition(char *cond_attribute, ASTNode *condition_node){
    // Implement the logic to decide the condition based on the attributes and the condition node
    //GT var op1 op2
    ASTNODE *op1 = root->left; // Assuming the left child holds the condition
    Token *op1_attr = get_token_attributes(condition_node); // Assuming the left child holds the condition

    ASTNODE *op2 = root->right; // Assuming the right child holds the condition
    Token *op2_attr = get_token_attributes(condition_node); // Assuming the left child holds the condition

    //condition name should be passed down to gen_if or gen_while to generate the correct jump labels

    switch(cond_attribute){
        case "GT":
            gen_GT(variable_gen(condition_node), op1_attr->attribute, op2_attr->attribute);
            break;
        case "LT":
            gen_LT(variable_gen(condition_node), op1_attr->attribute, op2_attr->attribute);
            break;
        case "EQ":
            gen_EQ(variable_gen(condition_node), op1_attr->attribute, op2_attr->attribute);
            break;
        default:
            // Handle other conditions if necessary
            break;

    }
}


void gen_while(ASTNode *root){
    // Similar to gen_if, but with a loop structure
    ASTNODE *condition_node = root->left; // Assuming the left child holds the condition
    Token *token_attribute = get_token_attributes(condition_node); // Assuming the left child holds the condition

    gen_label("$while_start");
    decide_condition(token_attribute->attribute, condition_node);
    gen_jumpifeq("$while_end", variable_gen(condition_node), "bool@false");

    gen_code(root->right); // Assuming the right child holds the loop body
    gen_jump("$while_start");

    gen_label("$while_end");
}

// aj tieto data typy musim z niakadial tahat. nejako to musim napojit
void gen_data_type(ASTNode *root){
    //v případě zápisu konstant používejte v jazyce C formátovací řetězec '%a' 
    Token token = get_token_attributes(root);

    switch (root->data_type) {
        case NIL://asi null?
            printf("nil@%s", token.value);
            break;
        case INT:
            printf("int@%d", token.value);
            break;
        case FLOAT:
            printf("float@%f", token.value);
            break;
        case STRING:
            printf("string@%s", token.value);
            break;
        case BOOL:  
             printf("bool@%s", token.value);
            break;
        case ARRAY:  
            printf("array@%s", token.value);
            break;
        //potrebuje to defaultnu hodnotu?
        default:
            printf("void@%s", token.value); /// toto asi neskor odstranit
    }
}


//TODO ako zistit ci je to GF alebo nie
// Toto je zle zrejme 
void gen_var(ASTNode *root){
    Token token = get_token_attributes(root);
    switch (root->data_type) {
        case GF://asi null?
            printf("GF@%s", scope, token.value);
            break;
        case LF:
            printf("LF@%s", scope, token.value);
            break;
        case TF:
            printf("TF@%s", scope, token.value);  
            break;
        default:
            char *scope = "void";  /// toto asi neskor odstranit
    }
}

void gen_move(char *var, char *symb){
    printf("MOVE %s %s\n", var, symb);
}

void gen_createframe(){
    printf("CREATEFRAME\n");
}

void gen_pushframe(){
    printf("PUSHFRAME\n");
}

void gen_popframe(){
    printf("POPFRAME\n");
}

void gen_defvar(char *var){
    printf("DEFVAR %s\n", var);
}

void gen_call(char *label){
    printf("CALL %s\n", label);
}

void gen_return(){
    printf("RETURN\n");
}

//je var 
//TODO data_type
void gen_pushs(char *symb, data_type type){
    if(type == identifier){
        printf("PUSHS %s\n", gen_var(symb));
    } else {
        printf("PUSHS %s\n", gen_data_type(symb););
    }
}

void gen_pops(char *var){
    printf("POPS %s\n", var);
}

void gen_clears(){
    printf("CLEARS\n");
}

void gen_add(char *var, char *symb1, char *symb2){
    printf("ADD %s %s %s\n", var, symb1, symb2);
}

void gen_sub(char *var, char *symb1, char *symb2){
    printf("SUB %s %s %s\n", var, symb1, symb2);
}

void gen_mul(char *var, char *symb1, char *symb2){
    printf("MUL %s %s %s\n", var, symb1, symb2);
}

void gen_div(char *var, char *symb1, char *symb2){
    printf("DIV %s %s %s\n", var, symb1, symb2);
}

void gen_idiv(char *var, char *symb1, char *symb2){
    printf("IDIV %s %s %s\n", var, symb1, symb2);
}

void gen_LT(char *var, char *symb1, char *symb2){
    printf("LT %s %s %s\n", var, symb1, symb2);
}

void gen_GT(char *var, char *symb1, char *symb2){
    printf("GT %s %s %s\n", var, symb1, symb2);
}

void gen_EQ(char *var, char *symb1, char *symb2){
    printf("EQ %s %s %s\n", var, symb1, symb2);
}

void gen_AND(char *var, char *symb1, char *symb2){
    printf("AND %s %s %s\n", var, symb1, symb2);
}

void gen_OR(char *var, char *symb1, char *symb2){
    printf("OR %s %s %s\n", var, symb1, symb2);
}

void gen_NOT(char *var, char *symb1){
    printf("NOT %s %s\n", var, symb1);
}

void gen_int2float(char *var, char *symb){
    printf("INT2FLOAT %s %s\n", var, symb);
}

void gen_float2int(char *var, char *symb){
    printf("FLOAT2INT %s %s\n", var, symb);
}

void gen_int2char(char *var, char *symb){
    printf("INT2CHAR %s %s\n", var, symb);
}

void gen_str2int(char *var, char *symb1, char *symb2){
    printf("STRR2INT %s %s %s\n", var, symb1, symb2);
}

void gen_float2str(char *var, char *symb){
    printf("FLOAT2STR %s %s\n", var, symb);
}

void gen_int2str(char *var, char *symb){
    printf("INT2STR %s %s\n", var, symb);
}

void gen_read(char *var, char *type){
    printf("READ %s %s\n", var, type);
}

void gen_write(char *symb){
    //TODO: musim tu riesit aj datovy typ, lebo sa to bude lisit podla toho co sa bude vypisovat
    printf("WRITE %s\n", symb);
}

void gen_concat(char *var, char *symb1, char *symb2){
    printf("CONCAT %s %s %s\n", var, symb1, symb2);
}

void gen_strlen(char *var, char *symb){
    printf("STRLEN %s %s\n", var, symb);
}

void gen_getchar(char *var, char *symb1, char *symb2){
    printf("GETCHAR %s %s %s\n", var, symb1, symb2);
}

void gen_setchar(char *var, char *symb1, char *symb2){
    printf("SETCHAR %s %s %s\n", var, symb1, symb2);
}

void gen_newarray(char *var, char *symbs){
    printf("NEWARRAY %s %s\n", var, symbs);
}

void gen_arrayget(char *var, char *var_a, char *symb_index){
    printf("ARRAYGET %s %s %s\n", var, var_a, symb_index);
}

void gen_arrayset(char *var_a, char *symb_index, char *symb_val){
    printf("ARRAYSET %s %s %s\n", var_a, symb_index, symb_val);
}

void gen_type(char *var, char *symb){
    printf("TYPE %s %s\n", var, symb);
}

void gen_isint(char *var, char *symb){
    printf("ISINT %s %s\n", var, symb);
}

void gen_label(char *label){
    printf("LABEL %s\n", label);
}

void gen_jump(char *label){
    printf("JUMP %s\n", label);
}

void gen_jumpifeq(char *label, char *symb1, char *symb2){
    printf("JUMPIFEQ %s %s %s\n", label, symb1, symb2);
}

void gen_jumpifneq(char *label, char *symb1, char *symb2){
    printf("JUMPIFNEQ %s %s %s\n", label, symb1, symb2);
}

void gen_exit(char *symb){
    printf("EXIT %s\n", symb);
}

void gen_break(){
    printf("BREAK\n");
}

void gen_dprint(char *symb){
    printf("DPRINT %s\n", symb);
}

void gen_adds(void){
    printf("ADDS\n");
}

void gen_subs(void){
    printf("SUBS\n");
}

void gen_muls(void){
    printf("MULS\n");
}

void gen_divs(void){
    printf("DIVS\n");
}

void gen_idivs(void){
    printf("IDIVS\n");
}

void gen_LTs(void){
    printf("LTS\n");
}

void gen_GTs(void){
    printf("GTS\n");
}

void gen_EQs(void){
    printf("EQS\n");
}

void gen_ANDs(void){
    printf("ANDS\n");
}

void gen_ORs(void){
    printf("ORS\n");
}

void gen_NOTs(void){
    printf("NOTS\n");
}

void gen_int2floats(void){
    printf("INT2FLOATS\n");
}

void gen_float2ints(void){
    printf("FLOAT2INTS\n");
}

void gen_int2chars(void){
    printf("INT2CHARS\n");
}

void gen_stri2ints(void){
    printf("STRI2INTS\n");
}

void gen_float2strs(void){
    printf("FLOAT2STRS\n");
}

void gen_int2strs(void){
    printf("INT2STRS\n");
}

void gen_types(void){
    printf("TYPES\n");
}

void gen_isints(void){
    printf("ISINTS\n");
}

void gen_jumpifeqs(char *label){
    printf("JUMPIFEQS %s\n", label);
}

void gen_jumpifneqs(char *label){
    printf("JUMPIFNEQS %s\n", label);
}