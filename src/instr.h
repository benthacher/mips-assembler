#pragma once

#include <stdint.h>
#include <stdio.h>

typedef enum {
    R_TYPE,
    I_TYPE,
    J_TYPE
} InstrType;

// instruction struct, contains fields for all types of instructions
// gets packed down to 32 bits by a corresponding type function
typedef struct {
    InstrType type;  // type of instruction
    uint32_t target; // J type
    uint16_t imm;    // I type
    uint16_t shamt;  // R type; 16 bits because it's immediate, but throws error if value is over 5 bits
    uint8_t opcode;  // all types
    uint8_t funct;   // R type
    uint8_t rs;      // R type
    uint8_t rt;      // R type
    uint8_t rd;      // R type
} instr_t;

// positions of all register parameters
#define INSTR_TARGET_POS (0)
#define INSTR_TARGET_MSK ((1 << 26) - 1)
#define INSTR_IMM_POS    (0)
#define INSTR_IMM_MSK    ((1 << 16) - 1)
#define INSTR_OPCODE_POS (26)
#define INSTR_OPCODE_MSK ((1 << 6) - 1)
#define INSTR_FUNCT_POS  (0)
#define INSTR_FUNCT_MSK  ((1 << 6) - 1)
#define INSTR_RS_POS     (21)
#define INSTR_RS_MSK     ((1 << 5) - 1)
#define INSTR_RT_POS     (16)
#define INSTR_RT_MSK     ((1 << 5) - 1)
#define INSTR_RD_POS     (11)
#define INSTR_RD_MSK     ((1 << 5) - 1)
#define INSTR_SHAMT_POS  (6)
#define INSTR_SHAMT_MSK  ((1 << 5) - 1)

typedef enum {
    /* R-type */
    INVALID = -1,
    ADD,
    ADDU,
    AND,
    BREAK,
    DIV,
    DIVU, 
    JALR, 
    JR,
    MFHI,
    MFLO,
    MTHI,
    MTLO,
    MULT,
    MULTU, 
    NOR,
    OR,
    SLL,
    SLLV,
    SLT,
    SLTU,
    SRA,
    SRAV,
    SRL,
    SRLV,
    SUB,
    SUBU, 
    SYSCALL,
    XOR,
    /* J-type */
    J,
    JAL,
    /* I-type */
    ADDI,
    ADDIU,
    ANDI,
    BEQ,
    BGEZ,
    BGTZ,
    BLEZ,
    BLTZ,
    BNE,
    LB,
    LBU,
    LH,
    LHU,
    LUI,
    LW,
    LWCL,
    ORI,
    SB,
    SLTI,
    SLTIU,
    SH,
    SW,
    SWCL,
    XORI
} InstrID;

typedef enum {
    RS,
    RD,
    RD_RS,
    RS_RT,
    RD_RS_RT,
    RD_RT_RS,
    RD_RT_SA,
    LABEL,
    RT_RS_IMM,
    RS_RT_LABEL,
    RS_LABEL,
    RT_IMM_RS,
    RT_IMM,
    NONE
} ParamOrder;

// J Types have one param order, LABEL

// ID bound #defines
#define NUM_INSTR (XORI + 1)
#define R_TYPE_START (ADD)
#define R_TYPE_END (XOR)
#define R_TYPE_LEN (R_TYPE_END - R_TYPE_START + 1)
#define I_TYPE_START (ADDI)
#define I_TYPE_END (XORI)
#define I_TYPE_LEN (I_TYPE_END - I_TYPE_START + 1)
#define J_TYPE_START (J)
#define J_TYPE_END (JAL)
#define J_TYPE_LEN (J_TYPE_END - J_TYPE_START + 1)

// LUTs
extern const char *INSTRUCTIONS[];
extern const ParamOrder PARAM_ORDERS[];
extern const uint8_t OPCODES[];
extern const uint8_t FUNCTS[];

// Functions
int64_t pack_instr(const instr_t *instr);
InstrID find_instr(const char *str);
InstrType get_type(InstrID id);
int get_opcode(InstrID id);
int get_funct(InstrID id);