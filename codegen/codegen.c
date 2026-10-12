#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

void create_head(){
    printf(".IFJcode26\n");
    printf("JUMP $main\n\n");
}

void tape_loop(Tape3AC *tape){
    while(tape->top >= 0){
        continue;
    }
}
