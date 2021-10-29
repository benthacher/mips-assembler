#include <stdio.h>
#include "assemble.h"

int main() {
    int ret = assemble("./test.asm", "./test.bin");

    if (ret == -1) {
        printf("Assembly error.");
    }

    return ret;
}