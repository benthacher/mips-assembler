#include "instr.h"

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