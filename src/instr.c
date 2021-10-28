#include "instr.h"
#include <string.h>
#include <stdlib.h>

const char* INSTRUCTIONS[] = {
    /* R-type */
    "add",
    "addu",
    "and",
    "break",
    "div",
    "divu", 
    "jalr", 
    "jr",
    "mfhi",
    "mflo",
    "mthi",
    "mtlo",
    "mult",
    "multu", 
    "nor",
    "or",
    "sll",
    "sllv",
    "slt",
    "sltu",
    "sra",
    "srav",
    "srl",
    "srlv",
    "sub",
    "subu", 
    "syscall",
    "xor",
    /* J-type */
    "j",
    "jal",
    /* I-type */
    "addi",
    "addiu",
    "andi",
    "beq",
    "bgez",
    "bgtz",
    "blez",
    "bltz",
    "bne",
    "lb",
    "lbu",
    "lh",
    "lhu",
    "lui",
    "lw",
    "lwcl",
    "ori",
    "sb",
    "slti",
    "sltiu",
    "sh",
    "sw",
    "swcl",
    "xori"
};

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

uint32_t pack_instr(const instr_t *instr) {
    // based on instruction type, pack word differently
    switch (instr->type) {
        case R_TYPE:
            return (
                ((instr->opcode & INSTR_OPCODE_MSK) << INSTR_OPCODE_POS) | 
                ((instr->rs & INSTR_RS_MSK) << INSTR_RS_POS) | 
                ((instr->rt & INSTR_RT_MSK) << INSTR_RT_POS) | 
                ((instr->rd & INSTR_RD_MSK) << INSTR_RD_POS) | 
                ((instr->shamt & INSTR_SHAMT_MSK) << INSTR_SHAMT_POS) | 
                ((instr->funct & INSTR_FUNCT_MSK) << INSTR_FUNCT_POS)
            );
            
        case I_TYPE:
            return (
                ((instr->opcode & INSTR_OPCODE_MSK) << INSTR_OPCODE_POS) | 
                ((instr->rs & INSTR_RS_MSK) << INSTR_RS_POS) | 
                ((instr->rt & INSTR_RT_MSK) << INSTR_RT_POS) | 
                ((instr->imm & INSTR_IMM_MSK) << INSTR_IMM_POS)
            );

        case J_TYPE:
            return (
                ((instr->opcode & INSTR_OPCODE_MSK) << INSTR_OPCODE_POS) | 
                ((instr->target & INSTR_TARGET_MSK) << INSTR_TARGET_POS)
            );
    }
}

/**
 * loop through instructions and try to find str
 * return the ID if found, invalid (-1) if not
 */
InstrID find_instr(const char *str) {
    for (int i = 0; i < NUM_INSTR; i++) {
        if (strcmp(str, INSTRUCTIONS[i]) == 0)
            return i;
    }
    return INVALID;
}

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