#include "assemble.h"
#include "instr.h"
#include "register.h"
#include <string.h>
#include <stdlib.h>

// Three static char buffers to put the params in while assembling
static char param1[MAX_PARAM_LENGTH + 1] = { [MAX_PARAM_LENGTH] = '\0' };
static char param2[MAX_PARAM_LENGTH + 1] = { [MAX_PARAM_LENGTH] = '\0' };
static char param3[MAX_PARAM_LENGTH + 1] = { [MAX_PARAM_LENGTH] = '\0' };

static inline void clear_params() {
    param1[MAX_PARAM_LENGTH] = '\0';
    param2[MAX_PARAM_LENGTH] = '\0';
    param3[MAX_PARAM_LENGTH] = '\0';
}

static inline int iswhitespace(char c) { return c == ' ' || c == '\n' || c == '\t' || c == -1; }

static inline int buffer_past_whitespace(FILE *fp) {
    char c;
    while ((c = fgetc(fp)) != EOF && iswhitespace(c)); // read one byte, if it's whitespace, continue, else break

    if (c == EOF)
        return EOF;

    fseek(fp, -1, SEEK_CUR); // move the file pointer back one byte
    return 0;
}

/**
 * next_str function
 * @param fp File pointer
 * @param buffer string buffer pointer
 * @param len Max length of buffer
 * @param terminator char that ends the search for the string
 * @return 0 if successfully found string, -1 if EOF, and -2 if a different char was found
 */
#define STR_NOT_FOUND (-2)
// just use the null terminator to denote whitespace as a terminator
#define WHITESPACE ('\0')
static int next_str(FILE *fp, char *buffer, int len, char terminator) {
    if (len <= 0) return -1;
    
    int start = ftell(fp); // get starting position of pointer

    int i = 0;
    char c;
    while ((c = fgetc(fp)) != terminator && c != EOF && i < len) {
        if (terminator == WHITESPACE && iswhitespace(c))
            break;
        // if char is whitespace, just skip it (advance fp but don't add to buffer)
        if (!iswhitespace(c)) {
            *buffer++ = c;
            i++;
        }
    }

    *buffer = '\0'; // set last character to zero (overwrite terminator)

    if (i + 1 == len) {
        fseek(fp, start, SEEK_CUR);
        return STR_NOT_FOUND;
    }

    if (c == EOF) {
        fseek(fp, start, SEEK_SET);
        return EOF;
    }

    // fseek(fp, -1, SEEK_CUR); // skip back to before the terminator
    return 0;
}

typedef struct {
    int line;
    int col;
} fp_pos_t;

static inline fp_pos_t get_fp_pos(FILE *fp) {
    int pos = ftell(fp);

    fseek(fp, 0, SEEK_SET); // go to the beginning of the file
    
    char c;
    int line_count = 1;
    int col_count = 1;

    for (int i = 0; i < pos; i++) {
        col_count++;
        c = fgetc(fp);

        if (c == '\n') {
            line_count++;
            col_count = 1; // reset column counter
        }
    }

    // we've calculated line/col count and fp is now in the same position as before
    return (fp_pos_t) { line_count, col_count };
}

static void print_error(FILE *fp, const char *error_str, const char *other) {
    fp_pos_t pos = get_fp_pos(fp);
    printf("Error: '%s%s' at %d:%d\n", error_str, other, pos.line, pos.col);
}

static int try_find_register(FILE *fp, const char *buffer) {
    int reg = find_register(buffer);

    if (reg == -1)
        print_error(fp, "Malformatted register ", buffer);
    else if (reg == -2)
        print_error(fp, "Unknown register ", buffer);
    
    return reg;
}

#define FIND_IMM_ERR (UINT32_MAX)
static uint32_t try_find_immediate(FILE *fp, const char *buffer) {
    char* end;
    long number = strtol(buffer, &end, 0);
    
    if (*end == '\0') {
        if (number < ((1 << 16) - 1))
            return number;
        else
            print_error(fp, "Immediate value too large!", buffer);
    } else
        print_error(fp, "Value is not a valid number", buffer);
    
    return FIND_IMM_ERR;
}

// fills the param1 char buffer
static int get_one_param(FILE *fp) {
    clear_params();
    
    if (buffer_past_whitespace(fp) == EOF)
        return -1;

    // get next string into param1
    if (next_str(fp, param1, MAX_PARAM_LENGTH, ' ') == 0)
        return 0;
    else {
        print_error(fp, "Param not found", "");
        return -1;
    }
}

static int get_two_params(FILE *fp) {
    clear_params();
    // skip any whitespace after instruction
    if (buffer_past_whitespace(fp) == EOF)
        return -1;

    // look for a comma
    int ret = next_str(fp, param1, MAX_PARAM_LENGTH, ',');
    
    if (ret == EOF) {
        if (feof(fp))
            print_error(fp, "Missing param separator", "");
        else
            print_error(fp, "Param is too long!", "");
        return -1;
    } else if (ret == STR_NOT_FOUND) {
        // if string not found, we found another character
        char c[2] = { fgetc(fp), '\0' };
        print_error(fp, "Random character", c);
        return -1;
    } else {
        // else we found the comma, find the next param
        buffer_past_whitespace(fp);

        // find whitespace
        ret = next_str(fp, param2, MAX_PARAM_LENGTH, WHITESPACE);

        if (ret == EOF) {
            if (feof(fp))
                print_error(fp, "Missing param separator", "");
            else
                print_error(fp, "Param is too long!", "");
            return -1;
        } else if (ret == STR_NOT_FOUND) {
            // if string not found, we found another character
            char c[2] = { fgetc(fp), '\0' };
            print_error(fp, "Random character", c);
            return -1;
        }
    }
    return 0;
}

static int get_three_params(FILE *fp) {
    clear_params();
    // skip any whitespace after instruction
    if (buffer_past_whitespace(fp) == EOF)
        return -1;

    // look for a comma
    int ret = next_str(fp, param1, MAX_PARAM_LENGTH, ',');
    
    if (ret == EOF) {
        if (feof(fp))
            print_error(fp, "Missing param separator", "");
        else
            print_error(fp, "Param is too long!", "");
        return -1;
    } else if (ret == STR_NOT_FOUND) {
        // if string not found, we found another character
        char c[2] = { fgetc(fp), '\0' };
        print_error(fp, "Random character", c);
        return -1;
    } else {
        // else we found the comma, find the next param
        buffer_past_whitespace(fp);

        // look for comma
        ret = next_str(fp, param2, MAX_PARAM_LENGTH, ',');

        if (ret == EOF) {
            if (feof(fp))
                print_error(fp, "Missing param separator", "");
            else
                print_error(fp, "Param is too long!", "");
            return -1;
        } else if (ret == STR_NOT_FOUND) {
            // if string not found, we found another character
            char c[2] = { fgetc(fp), '\0' };
            print_error(fp, "Random character", c);
            return -1;
        } else {
            // else we found the comma, find the next param
            buffer_past_whitespace(fp);

            // find whitespace
            ret = next_str(fp, param3, MAX_PARAM_LENGTH, WHITESPACE);

            if (ret == EOF) {
                if (feof(fp))
                    print_error(fp, "Missing param separator", "");
                else
                    print_error(fp, "Param is too long!", "");
                return -1;
            } else if (ret == STR_NOT_FOUND) {
                // if string not found, we found another character
                char c[2] = { fgetc(fp), '\0' };
                print_error(fp, "Random character", c);
                return -1;
            }
        }
    }
    return 0;
}

static int get_one_reg(FILE *fp, uint8_t *reg) {
    // try to get one param into param1
    if (get_one_param(fp) != 0)
        return -1;
    
    int res = try_find_register(fp, param1);

    if (res > -1)
        *reg = res;
    else
        return -1;

    return 0;
}

static int get_two_regs(FILE *fp, uint8_t *reg1, uint8_t *reg2) {
    // try to get two params
    if (get_two_params(fp) != 0)
        return -1;    

    int res = try_find_register(fp, param1);

    if (res > -1)
        *reg1 = res;
    else
        return -1;
    
    res = try_find_register(fp, param2);

    if (res > -1)
        *reg2 = res;
    else
        return -1;

    return 0;
}

static int get_three_regs(FILE *fp, uint8_t *reg1, uint8_t *reg2, uint8_t *reg3) {
    // try to get two params
    if (get_three_params(fp) != 0)
        return -1;    

    int res = try_find_register(fp, param1);

    if (res > -1)
        *reg1 = res;
    else
        return -1;
    
    res = try_find_register(fp, param2);

    if (res > -1)
        *reg2 = res;
    else
        return -1;

    res = try_find_register(fp, param3);

    if (res > -1)
        *reg3 = res;
    else
        return -1;

    return 0;
}

static int get_two_regs_and_imm(FILE *fp, uint8_t *reg1, uint8_t *reg2, int16_t *imm) {
    // try to get three params
    if (get_three_params(fp) != 0)
        return -1;    

    int res = try_find_register(fp, param1);

    if (res > -1)
        *reg1 = res;
    else
        return -1;
    
    res = try_find_register(fp, param2);

    if (res > -1)
        *reg2 = res;
    else
        return -1;
    
    res = try_find_immediate(fp, param3);

    if (res != FIND_IMM_ERR)
        *imm = res;
    else
        return -1;
    
    return 0;
}

static int get_regs_and_offset(FILE *fp, uint8_t *reg1, uint8_t *reg2, int16_t *offs) {
    // try to get two params
    if (get_two_params(fp) != 0)
        return -1;    

    // first param is the first reg
    int res = try_find_register(fp, param1);

    if (res > -1)
        *reg1 = res;
    else
        return -1;
    
    // second param looks like this: offs(reg), so we just find the opening parenthesis and have fun
    // read from param2 start to index of (
    
    char *open_paren = strchr(param2, '(');

    if (open_paren == NULL) {
        print_error(fp, "Malformatted offset", param2);
        return -1;
    }

    *open_paren = '\0'; // turn the ( into a null terminator so we can read an immediate from it

    res = try_find_immediate(fp, param2);

    if (res != FIND_IMM_ERR)
        *offs = res;
    else
        return -1;
    
    // read the register
    // first we find the closing parenthesis by starting at the opening one
    char *close_paren = strchr(open_paren + 1, ')');

    if (open_paren == NULL) {
        print_error(fp, "Malformatted source register", param2);
        return -1;
    }

    *close_paren = '\0'; // turn the ( into a null terminator so we can read an immediate from it

    // second register is between the open and close parenthesis
    res = try_find_register(fp, open_paren + 1);

    if (res > -1)
        *reg2 = res;
    else
        return -1;

    return 0;
}

static int get_one_reg_and_imm(FILE *fp, uint8_t *reg1, int16_t *imm) {
    // try to get three params
    if (get_two_params(fp) != 0)
        return -1;

    int res = try_find_register(fp, param1);

    if (res > -1)
        *reg1 = res;
    else
        return -1;
    
    res = try_find_immediate(fp, param2);

    if (res != FIND_IMM_ERR)
        *imm = res;
    else
        return -1;

    return 0;
}

static int set_params(InstrID id, FILE *fp, instr_t *instr) {
    ParamOrder order = PARAM_ORDERS[id];
    
    // if no params, just return, don't set any params
    if (order == NONE)
        return 0;

    int ret = 0;

    // Get param order based on id, parse params
    switch (order) {
        case RS:        ret = get_one_reg(fp, &instr->rs); break;
        case RD:        ret = get_one_reg(fp, &instr->rd); break;
        case RD_RS:     ret = get_two_regs(fp, &instr->rd, &instr->rs); break;
        case RS_RT:     ret = get_two_regs(fp, &instr->rs, &instr->rt); break;
        case RD_RS_RT:  ret = get_three_regs(fp, &instr->rd, &instr->rs, &instr->rt); break;
        case RD_RT_RS:  ret = get_three_regs(fp, &instr->rd, &instr->rt, &instr->rd); break;
        case RD_RT_SA:  ret = get_two_regs_and_imm(fp, &instr->rd, &instr->rt, &instr->shamt); break;
        case LABEL: break;
        case RT_RS_IMM: ret = get_two_regs_and_imm(fp, &instr->rt, &instr->rs, &instr->imm); break;
        case RS_RT_LABEL: break;
        case RS_LABEL: break;
        case RT_IMM_RS: ret = get_regs_and_offset(fp, &instr->rt, &instr->rs, &instr->imm); break;
        case RT_IMM:    ret = get_one_reg_and_imm(fp, &instr->rt, &instr->imm); break;
    }

    return ret;
}

/**
 * Takes a file pointer to the assembly file, advances the pointer, and returns a packed instruction
 * Assume we've read the instruction from the file already, so we have the id
 */
static int64_t construct_instruction(FILE *fp) {
    instr_t instr;
    char c;

    char instr_str[10];
    // from current position to the next space
    next_str(fp, instr_str, 10, WHITESPACE);

    InstrID id = find_instr(instr_str);

    if (id == INVALID) {
        print_error(fp, "Unknown instruction ", instr_str);
        return -1;
    }

    // populate fields that we can after instruction 
    instr.opcode = get_opcode(id);
    instr.funct  = get_funct(id);
    instr.type   = get_type(id);

    instr.rs     = 0;
    instr.rt     = 0;
    instr.rd     = 0;
    instr.shamt  = 0;
    instr.imm    = 0;

    if (set_params(id, fp, &instr) == -1)
        return -1; // error occurred

    return pack_instr(&instr);
}

/**
 * Takes the paths of input and output files
 */
int assemble(const char *infile, const char *outfile) {
    FILE *fin = fopen(infile, "r");
    FILE *fout = fopen(outfile, "w");

    if (fin == NULL || fout == NULL)
        return -1;
    
    uint32_t pc = 0;
    while (1) {
        // assume we're looking for an instruction or label
        if (buffer_past_whitespace(fin) == EOF) {
            printf("Reached end of file.\n");
            break;
        }

        // we've found something, read the string
        char str[100];
        // look for a label
        if (next_str(fin, str, sizeof(str), ':') > 0) {
            // we found a label
            // do something with the label, then continue
            continue;
        }

        // label was not found, try to decode as instruction
        int64_t instr_code = construct_instruction(fin);

        if (instr_code == -1) {
            printf("Reached end of file but something bad happened :(\n");
            break;
        } else {
            uint32_t instr_code_packed = instr_code;
            fwrite(&instr_code_packed, sizeof(uint32_t), 1, fout);
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}