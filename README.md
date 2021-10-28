# MIPS Assembler

This assembler is meant to convert MIPS assembly to MIPS machine code.

To-do:
- [x] instr_t struct/union thing
- [ ] Instruction ID -> Instruction string map
- [ ] Instruction ID lookup function
- [ ] LUTs and functions for getting instruction attributes based on ID
- [ ] Pass 1 (big linked list for label/PC address lookup during pass 2)
- [ ] Pass 2, look through text and create instructions, shove them in binary file