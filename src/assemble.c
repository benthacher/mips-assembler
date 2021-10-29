#include "assemble.h"
#include "instr.h"
#include "register.h"

static inline int iswhitespace(char c) { return c == ' ' || c == '\n' || c == '\t'; }

static inline int buffer_past_whitespace(FILE *fp) {
    char c;
    while ((c = fgetc(fp)) != EOF && iswhitespace(c)); // read one byte, if it's whitespace, continue, else break

    if (c == EOF)
        return EOF;

    fseek(fp, -1, SEEK_CUR); // move the file pointer back one byte
    return 0;
}

#define STR_NOT_FOUND (-2)
static int next_str(FILE *fp, char *buffer, int len, char terminator) {
    if (len <= 0) return -1; // arbitrary max string size
    
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != terminator && c != EOF && !iswhitespace(c) && ++i < len) {
        *buffer++ = c;
    }

    if (i == len) {
        fseek(fp, -len, SEEK_CUR);
        return STR_NOT_FOUND;
    }

    if (c == EOF || (iswhitespace(c) && !iswhitespace(terminator))) {
        fseek(fp, -i - 1, SEEK_CUR);
        return EOF;
    }

    *buffer = '\0'; // set last character to zero (overwrite terminator)
    fseek(fp, -1, SEEK_CUR); // skip back to before the terminator
    return 0;
}

static int buffer_until_whitespace(FILE *fp, char *buffer, int len) {
    if (len <= 0) return -1;
    
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF && !iswhitespace(c) && ++i < len) {
        *buffer++ = c;
    }

    if (i == len) {
        fseek(fp, -len, SEEK_CUR);
        return STR_NOT_FOUND;
    }

    *buffer = '\0'; // set last character to zero (overwrite terminator)

    if (c == EOF)
        return EOF; // don't roll fp back if it's EOF, just return
    
    fseek(fp, -1, SEEK_CUR); // skip back to before the terminator
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

static int get_one_reg(FILE *fp, uint8_t *reg) {
    char arg[10];
    
    buffer_until_whitespace(fp, arg, sizeof(arg));

    int ret = try_find_register(fp, arg);

    if (ret > -1)
        *reg = ret;
    else
        return -1;
}

static int get_two_reg(FILE *fp, uint8_t *reg1, uint8_t *reg2) {
    char arg[10];
    
    // if comma can't be found, there might be a space between the 
    // first reg and the comma
    if (next_str(fp, arg, sizeof(arg), ',') == EOF) {
        // try to find a reg before the comma
        buffer_until_whitespace(fp, arg, sizeof(arg));

        int reg = try_find_register(fp, arg);

        if (reg > -1)
            *reg1 = reg; // first reg found!
        else
            return -1;

        buffer_past_whitespace(fp);

        // if next character is a comma, we can skip it and
        // look for the second reg
        if (fgetc(fp) == ',') {
            buffer_past_whitespace(fp);
            buffer_until_whitespace(fp, arg, sizeof(arg));

            int reg = try_find_register(fp, arg);

            if (reg > -1)
                *reg2 = reg; // second reg found!
            else
                return -1;
        } else {
            print_error(fp, "Missing second argument", "");
            return -1;
        }
    } else { // if next_str returned 0, then arg is the first reg
        int reg = try_find_register(fp, arg);

        if (reg > -1)
            *reg1 = reg; // first reg found!
        else
            return -1;
        
        fgetc(fp); // read the comma
        buffer_past_whitespace(fp);
        buffer_until_whitespace(fp, arg, sizeof(arg));

        reg = try_find_register(fp, arg);

        if (reg > -1)
            *reg2 = reg; // second reg found!
        else
            return -1;
    }

    return 0;
}

static int set_params(InstrID id, FILE *fp, instr_t *instr) {
    ParamOrder order = PARAM_ORDERS[id];
    
    // if no params, just return, don't set any params
    if (order == NONE)
        return 0;

    int ret = 0;

    // in all other cases, skip whitespace
    buffer_past_whitespace(fp);

    // Get param order based on id, parse params
    switch (order) {
        case RS:    ret = get_one_reg(fp, &instr->rs); break;
        case RD:    ret = get_one_reg(fp, &instr->rd); break;
        case RD_RS: ret = get_two_reg(fp, &instr->rd, &instr->rs); break;
        case RS_RT: ret = get_two_reg(fp, &instr->rs, &instr->rt); break;
        case RD_RS_RT:

            break;
        case RD_RT_RS:

            break;
        case RD_RT_SA:

            break;
        case LABEL:

            break;
        case RT_RS_IMM:

            break;
        case RS_RT_LABEL:

            break;
        case RS_LABEL:

            break;
        case RT_IMM_RS:

            break;
        case RT_IMM:

            break;
    }

    return ret;
}

/**
 * Takes a file pointer to the assembly file, advances the pointer, and returns a packed instruction
 * Assume we've read the instruction from the file already, so we have the id
 */
static int32_t construct_instruction(FILE *fp) {
    instr_t instr;
    char c;

    char instr_str[10];
    // from current position to the next space
    next_str(fp, instr_str, 10, ' ');

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
            printf("Reached end of file.");
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
        int32_t instr_code = construct_instruction(fin);

        if (instr_code == -1)
            break;
        else
            fwrite(&instr_code, sizeof(uint32_t), 1, fout);
    }

    fclose(fin);
    fclose(fout);
    return 0;
}