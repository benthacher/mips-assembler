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