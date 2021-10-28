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

const uint8_t OPCODES[] = {
    0b000010, /* J */
    0b000011, /* JAL */
    0b001000, /* ADDI */
    0b001001, /* ADDIU */
    0b001100, /* ANDI */
    0b000100, /* BEQ */
    0b000001, /* BGEZ */
    0b000111, /* BGTZ */
    0b000110, /* BLEZ */
    0b000001, /* BLTZ */
    0b000101, /* BNE */
    0b100000, /* LB */
    0b100100, /* LBU */
    0b100001, /* LH */
    0b100101, /* LHU */
    0b001111, /* LUI */
    0b100011, /* LW */
    0b110001, /* LWCL */
    0b001101, /* ORI */
    0b101000, /* SB */
    0b001010, /* SLTI */
    0b001011, /* SLTIU */
    0b101001, /* SH */
    0b101011, /* SW */
    0b111001, /* SWCL */
    0b001110, /* XORI */
};

const uint8_t FUNCTS[] = {
    0b100000, /* ADD */
    0b100001, /* ADDU */
    0b100100, /* AND */
    0b001101, /* BREAK */
    0b011010, /* DIV */
    0b011011, /* DIVU */ 
    0b001001, /* JALR */ 
    0b001000, /* JR */
    0b010000, /* MFHI */
    0b010010, /* MFLO */
    0b010001, /* MTHI */
    0b010011, /* MTLO */
    0b011000, /* MULT */
    0b011001, /* MULTU */ 
    0b100111, /* NOR */
    0b100101, /* OR */
    0b000000, /* SLL */
    0b000100, /* SLLV */
    0b101010, /* SLT */
    0b101011, /* SLTU */
    0b000011, /* SRA */
    0b000111, /* SRAV */
    0b000010, /* SRL */
    0b000110, /* SRLV */
    0b100010, /* SUB */
    0b100011, /* SUBU */ 
    0b001100, /* SYSCALL */
    0b100110, /* XOR */
};

const RTypeParamOrder R_TYPE_ORDER[] = {
    RD_RS_RT, /* ADD */
    RD_RS_RT, /* ADDU */
    RD_RS_RT, /* AND */
    NONE, /* BREAK */
    RS_RT, /* DIV */
    RS_RT, /* DIVU */ 
    RD_RS, /* JALR */ 
    RS, /* JR */
    RD, /* MFHI */
    RD, /* MFLO */
    RS, /* MTHI */
    RS, /* MTLO */
    RS_RT, /* MULT */
    RS_RT, /* MULTU */ 
    RD_RS_RT, /* NOR */
    RD_RS_RT, /* OR */
    RD_RT_SA, /* SLL */
    RD_RT_RS, /* SLLV */
    RD_RS_RT, /* SLT */
    RD_RS_RT, /* SLTU */
    RD_RT_SA, /* SRA */
    RD_RT_RS, /* SRAV */
    RD_RT_SA, /* SRL */
    RD_RT_RS, /* SRLV */
    RD_RS_RT, /* SUB */
    RD_RS_RT, /* SUBU */ 
    NONE, /* SYSCALL */
    RD_RS_RT, /* XOR */
};

const ITypeParamOrder I_TYPE_ORDER[] = {
    RT_RS_IMM, /* ADDI */
    RT_RS_IMM, /* ADDIU */
    RT_RS_IMM, /* ANDI */
    RS_RT_LABEL, /* BEQ */
    RS_LABEL, /* BGEZ */
    RS_LABEL, /* BGTZ */
    RS_LABEL, /* BLEZ */
    RS_LABEL, /* BLTZ */
    RS_RT_LABEL, /* BNE */
    RT_IMM_RS, /* LB */
    RT_IMM_RS, /* LBU */
    RT_IMM_RS, /* LH */
    RT_IMM_RS, /* LHU */
    RT_IMM, /* LUI */
    RT_IMM_RS, /* LW */
    RT_IMM_RS, /* LWCL */
    RT_RS_IMM, /* ORI */
    RT_IMM_RS, /* SB */
    RT_RS_IMM, /* SLTI */
    RT_RS_IMM, /* SLTIU */
    RT_IMM_RS, /* SH */
    RT_IMM_RS, /* SW */
    RT_IMM_RS, /* SWCL */
    RT_RS_IMM, /* XORI */
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
 * Returns the type of an instruction given the id
 */
InstrType get_type(InstrID id) {
    if (id >= R_TYPE_START && id <= R_TYPE_END)
        return R_TYPE;
    if (id >= I_TYPE_START && id <= I_TYPE_END)
        return I_TYPE;
    if (id >= J_TYPE_START && id <= J_TYPE_END)
        return J_TYPE;
}

/**
 * Find opcode based on InstrID
 */
int get_opcode(InstrID id) {
    if (get_type(id) == R_TYPE)
        return 0; // all R-type instructions have opcode 0

    // type must be J_TYPE or I_TYPE
    // we must subtract out the number of R-type instructions
    // because J and I type IDs are offset by the number of R type instructions
    return OPCODES[id - R_TYPE_LEN];
}

/**
 * Find funct code based on InstrID
 */
int get_funct(InstrID id) {
    if (get_type(id) != R_TYPE)
        return 0; // all non R-type instructions don't use funct code, just set to zero

    // type must be R-type
    return FUNCTS[id];
}

/**
 * Lookup parameter order
 */
int get_param_order(InstrID id) {
    switch (get_type(id)) {
        case R_TYPE:
            return R_TYPE_ORDER[id];
        case I_TYPE:
            return I_TYPE_ORDER[id - R_TYPE_LEN - J_TYPE_LEN];
        default:
            return -1;
    }
}