#include "tests.h"
#include "hashtable_tests/hashtable_func.h"
#include "codegen_tests/codegen_tests.h"

int main() {
    hashtable_tests();
    symtable_tests();
    codegen_tests();

    return 0;
}