#include "codegen_helper.h"

Token get_token_from_node(ASTNode *node) {
    Token token = node->val;
    return token;
}

char* string_from_token(ASTNode *node) {
    Token token = get_token_from_node(node);
    return token.attribute.string_value;
}


extern char *current_func_name; 

char *create_label(int number, char* label_name, char* label_type) {
    int len = snprintf(NULL, 0, "$%s$%s$%d$%s", current_func_name, label_name, number, label_type) + 1;
    if (len <= 0) return NULL;

    char *label = malloc((size_t)len);
    if (label == NULL) return NULL;

    snprintf(label, (size_t)len, "$%s$%s$%d$%s", current_func_name, label_name, number, label_type);

    return label; // Remember to free(else_label) when done!
}