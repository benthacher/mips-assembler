#include <stdio.h>
#include "instr.h"
#include "register.h"

int main() {
    char *str = "ad";

    int id = find_instr(str);
    int reg = find_register("$2");

    printf("reg: %d", reg);

    return 0;
}