#pragma once

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


uint32_t pack_instr(const instr_t *instr);