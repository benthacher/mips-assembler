#include "register.h"
#include <string.h>

const char* REGISTERS[] = {
    "0",
    "at",
    "v0",
    "v1",
    "a0",
    "a1",
    "a2",
    "a3",
    "t0",
    "t1",
    "t2",
    "t3",
    "t4",
    "t5",
    "t6",
    "t7",
    "s0",
    "s1",
    "s2",
    "s3",
    "s4",
    "s5",
    "s6",
    "s7",
    "t8",
    "t9",
    "k0",
    "k1",
    "gp",
    "sp",
    "fp",
    "ra",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",
    "22",
    "23",
    "24",
    "25",
    "26",
    "27",
    "28",
    "29",
    "30",
    "31",
};

/**
 * Lookup register based on string
 * Returns register code if register is found
 * Returns -1 for malformatted register string
 * Returns -2 for unknown register
 */
int find_register(const char *str) {
    if (str == NULL)
        return -1;
    // if first character isn't a $, the string is a malformatted register
    // increment str pointer to skip the $
    if (*str++ != '$')
        return -1;

    // There are 32 regs, but they can be represented by both their
    // functional names and by their ID. This is why I return the index & 0x1F
    for (int i = 0; i < NUM_REGS * 2; i++) {
        if (strcmp(str, REGISTERS[i]) == 0)
            return i & 0x1F;
    }
    return -2; // unknown register
}