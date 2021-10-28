#include <stdio.h>
#include "instr.h"
#include <stdlib.h>

int main() {
    char *str = "ad";

    int id = find_instr(str);
    int reg = find_register("");

    printf("reg: %d", reg);

    return 0;
}