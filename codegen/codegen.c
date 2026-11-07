#include "codegen.h"

void create_head(){
    printf(".IFJcode26\n");
    printf("JUMP $main\n\n");
}

void gen_node(ASTNode *root){
}

//Toto bude treba potom uvolnit
// aj tieto data typy musim z niakadial tahat. nejako to musim napojit
char *data_type(ASTNode *root){
    //v případě zápisu konstant používejte v jazyce C formátovací řetězec '%a' 
    switch (root->data_type) {
        case NIL://asi null?
            char *dataTypePrefix = "nil";
            break;
        case INT:
            char *dataTypePrefix = "int";
            break;
        case FLOAT:
            char *dataTypePrefix = "float";
            break;
        case STRING:
            char *dataTypePrefix = "string";
            break;
        case BOOL:  
            char *dataTypePrefix = "bool";
            break;
        case ARRAY:  
            char *dataTypePrefix = "array";
            break;
        //potrebuje to defaultnu hodnotu?
        default:
            char *dataTypePrefix = "void";  /// toto asi neskor odstranit
    }
    value = root->value; // Predpokladám, že ASTNode má pole 'value', ktoré obsahuje hodnotu typu string
    size_t len = strlen(dataTypePrefix) + strlen(value) + 2;
    
    char *result = malloc(len);
    if (result == NULL) {
        return NULL; // Alokácia zlyhala
    }

    // Bezpečne spojíme obe časti do nového reťazca
    snprintf(result, len, "%s@%s", dataTypePrefix, part2);
    
    return result;

}


char*variable_gen(ASTNode *root){

        switch (root->data_type) {
        case GF://asi null?
            char *scope = "GF";
            break;
        case LF:
            char *scope = "LF";
            break;
        case TF:
            char *scope = "TF";
            break;
        default:
            char *scope = "void";  /// toto asi neskor odstranit
    }

    char *value = root->value;
    size_t len = strlen(value) + 2; // +2 pre '$' a '\0'
    
    char *result = malloc(len);
    if (result == NULL) {
        return NULL; // Alokácia zlyhala
    }

    // Bezpečne spojíme '$' a hodnotu do nového reťazca
    snprintf(result, len, "s@%s",scope,  value);
    
    return result;
}

