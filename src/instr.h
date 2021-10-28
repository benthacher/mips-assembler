#ifndef INSTR_H
#define INSTR_H

#include <stdint.h>

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
    uint8_t opcode;  // all types
    uint8_t funct;   // R type
    uint8_t rs;      // R type
    uint8_t rt;      // R type
    uint8_t rd;      // R type
    uint8_t shamt;   // R type
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
    NONE
} RTypeParamOrder;

typedef enum {
    RT_RS_IMM,
    RS_RT_LABEL,
    RS_LABEL,
    RT_IMM_RS,
    RT_IMM
} ITypeParamOrder;

// J Types have one param order, LABEL

// ID bound #defines
#define NUM_INSTR (XORI + 1)
#define NUM_REGS (32)
#define R_TYPE_START (ADD)
#define R_TYPE_LEN (XOR - R_TYPE_START + 1)
#define I_TYPE_START (ADDI)
#define I_TYPE_LEN (XORI - I_TYPE_START + 1)
#define J_TYPE_START (J)
#define J_TYPE_LEN (JAL - J_TYPE_START + 1)

// LUTs
extern const char *INSTRUCTIONS[];
extern const char *REGISTERS[];
extern const RTypeParamOrder R_TYPE_ORDER[];
extern const ITypeParamOrder I_TYPE_ORDER[];
extern const uint8_t OPCODES[];
extern const uint8_t FUNCTS[];

// Functions
uint32_t pack_instr(const instr_t *instr);
InstrID find_instr(const char *str);
int find_register(const char *str);

#endif