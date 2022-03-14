#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "y64asm.h"

line_t *line_head = NULL;
line_t *line_tail = NULL;
int lineno = 0;

#define err_print(_s, _a ...) do { \
  if (lineno < 0) \
    fprintf(stderr, "[--]: "_s"\n", ## _a); \
  else \
    fprintf(stderr, "[L%d]: "_s"\n", lineno, ## _a); \
} while (0);


int64_t vmaddr = 0;    /* vm addr */

/* register table   name id namelen*/ 
const reg_t reg_table[REG_NONE] = {
    {"%rax", REG_RAX, 4},
    {"%rcx", REG_RCX, 4},
    {"%rdx", REG_RDX, 4},
    {"%rbx", REG_RBX, 4},
    {"%rsp", REG_RSP, 4},
    {"%rbp", REG_RBP, 4},
    {"%rsi", REG_RSI, 4},
    {"%rdi", REG_RDI, 4},
    {"%r8",  REG_R8,  3},
    {"%r9",  REG_R9,  3},
    {"%r10", REG_R10, 4},
    {"%r11", REG_R11, 4},
    {"%r12", REG_R12, 4},
    {"%r13", REG_R13, 4},
    {"%r14", REG_R14, 4}
};
const reg_t* find_register(char *name)
{
    int i;
    for (i = 0; i < REG_NONE; i++)
        if (!strncmp(name, reg_table[i].name, reg_table[i].namelen))
            return &reg_table[i];
    return NULL;
}


/* instruction set  name len code bytes*/
instr_t instr_set[] = {
    {"nop", 3,   HPACK(I_NOP, F_NONE), 1 },
    {"halt", 4,  HPACK(I_HALT, F_NONE), 1 },
    {"rrmovq", 6,HPACK(I_RRMOVQ, F_NONE), 2 },
    {"cmovle", 6,HPACK(I_RRMOVQ, C_LE), 2 },
    {"cmovl", 5, HPACK(I_RRMOVQ, C_L), 2 },
    {"cmove", 5, HPACK(I_RRMOVQ, C_E), 2 },
    {"cmovne", 6,HPACK(I_RRMOVQ, C_NE), 2 },
    {"cmovge", 6,HPACK(I_RRMOVQ, C_GE), 2 },
    {"cmovg", 5, HPACK(I_RRMOVQ, C_G), 2 },
    {"irmovq", 6,HPACK(I_IRMOVQ, F_NONE), 10 },
    {"rmmovq", 6,HPACK(I_RMMOVQ, F_NONE), 10 },
    {"mrmovq", 6,HPACK(I_MRMOVQ, F_NONE), 10 },
    {"addq", 4,  HPACK(I_ALU, A_ADD), 2 },
    {"subq", 4,  HPACK(I_ALU, A_SUB), 2 },
    {"andq", 4,  HPACK(I_ALU, A_AND), 2 },
    {"xorq", 4,  HPACK(I_ALU, A_XOR), 2 },
    {"jmp", 3,   HPACK(I_JMP, C_YES), 9 },
    {"jle", 3,   HPACK(I_JMP, C_LE), 9 },
    {"jl", 2,    HPACK(I_JMP, C_L), 9 },
    {"je", 2,    HPACK(I_JMP, C_E), 9 },
    {"jne", 3,   HPACK(I_JMP, C_NE), 9 },
    {"jge", 3,   HPACK(I_JMP, C_GE), 9 },
    {"jg", 2,    HPACK(I_JMP, C_G), 9 },
    {"call", 4,  HPACK(I_CALL, F_NONE), 9 },
    {"ret", 3,   HPACK(I_RET, F_NONE), 1 },
    {"pushq", 5, HPACK(I_PUSHQ, F_NONE), 2 },
    {"popq", 4,  HPACK(I_POPQ, F_NONE),  2 },
    {".byte", 5, HPACK(I_DIRECTIVE, D_DATA), 1 },
    {".word", 5, HPACK(I_DIRECTIVE, D_DATA), 2 },
    {".long", 5, HPACK(I_DIRECTIVE, D_DATA), 4 },
    {".quad", 5, HPACK(I_DIRECTIVE, D_DATA), 8 },
    {".pos", 4,  HPACK(I_DIRECTIVE, D_POS), 0 },
    {".align", 6,HPACK(I_DIRECTIVE, D_ALIGN), 0 },
    {NULL, 1,    0   , 0 } //end
};

instr_t *find_instr(char *name)
{
    int i;
    for (i = 0; instr_set[i].name; i++)
	if (strncmp(instr_set[i].name, name, instr_set[i].len) == 0)
	    return &instr_set[i];
    return NULL;
}

/* symbol table (don't forget to init and finit it) */
symbol_t *symtab = NULL;

/* 
 * find_symbol: scan table to find the symbol
 * args
 *     name: the name of symbol
 *
 * return
 *     symbol_t: the 'name' symbol
 *     NULL: not exist
 */
symbol_t *find_symbol(char *name)
{
    symbol_t * sym_pointer = symtab->next;
    while(sym_pointer)
    {
        if(sym_pointer->name&&!strcmp(sym_pointer->name,name))
            //find bug2: cannot do cmp for unallocated area?
            break;
        sym_pointer = sym_pointer->next;
    }
    return sym_pointer;
}

/*
 * add_symbol: add a new symbol to the symbol table
 * args
 *     name: the name of symbol
 *
 * return
 *     0: success
 *     -1: error, the symbol has exist
 */
int add_symbol(char *name)
{
    /* check duplicate */
    if(find_symbol(name)){
        return -1;//the symbol has exist
    }

    /* create new symbol_t (don't forget to free it)*/
    //symtab will be freed in finit
    symbol_t * new_symbol = (symbol_t *)malloc(sizeof(symbol_t));
    memset(new_symbol,0,sizeof(symbol_t));
    new_symbol->name = name;
    new_symbol->addr = vmaddr;
    new_symbol->next = symtab->next;
    /* add the new symbol_t to symbol table */
    symtab->next = new_symbol;
    return 0;
}

/* relocation table (don't forget to init and finit it) */
reloc_t *reltab = NULL;
/*
 * add_reloc: add a new relocation to the relocation table
 * args
 *     name: the name of symbol
 */
void add_reloc(char *name, bin_t *bin)
{
    /* create new reloc_t (don't forget to free it)*/
    //will be freed in finit
    reloc_t * new_reloc = (reloc_t *) malloc(sizeof(reloc_t));
    memset(new_reloc,0,sizeof(reloc_t));
    new_reloc->y64bin = bin;
    new_reloc->name = name;
    new_reloc->next = reltab->next;
    /* add the new reloc_t to relocation table */
    reltab->next = new_reloc;
}


/* macro for parsing y64 assembly code */
#define IS_DIGIT(s) ((*(s)>='0' && *(s)<='9') || *(s)=='-' || *(s)=='+')
#define IS_LETTER(s) ((*(s)>='a' && *(s)<='z') || (*(s)>='A' && *(s)<='Z'))
#define IS_COMMENT(s) (*(s)=='#')
#define IS_REG(s) (*(s)=='%')
#define IS_IMM(s) (*(s)=='$')

#define IS_BLANK(s) (*(s)==' ' || *(s)=='\t')
#define IS_END(s) (*(s)=='\0')

#define SKIP_BLANK(s) do {  \
  while(!IS_END(s) && IS_BLANK(s))  \
    (s)++;    \
} while(0);
# define SKIP_AND_CHECK(s) do{ \
    SKIP_BLANK(s);             \
    if(IS_END(s))              \
        return PARSE_ERR;      \
}while(0);

void write_bytes_little_endian(long value,byte_t * codes,int _bytes){
    for(int i = 0; i < _bytes; ++ i){
        codes[i] = (value & 0xFF);
        value >>= 8;
    }
}
/* return value from different parse_xxx function */
typedef enum { PARSE_ERR=-1, PARSE_REG, PARSE_DIGIT, PARSE_SYMBOL, 
    PARSE_MEM, PARSE_DELIM, PARSE_INSTR, PARSE_LABEL} parse_t;

/*
 * parse_instr: parse an expected data token (e.g., 'rrmovq')
 * args
 *     ptr: point to the start of string
 *     inst: point to the inst_t within instr_set
 *
 * return
 *     PARSE_INSTR: success, move 'ptr' to the first char after token,
 *                            and store the pointer of the instruction to 'inst'
 *     PARSE_ERR: error, the value of 'ptr' and 'inst' are undefined
 */
parse_t parse_instr(char **ptr, instr_t **inst)
{
    /* skip the blank ' '|| '\t' not '\0'*/
    //check end
    SKIP_AND_CHECK(*ptr);
    /* find_instr */
    instr_t * found_instr = find_instr(*ptr);

    /* set 'ptr' and 'inst' */
    if(found_instr){
        *ptr += found_instr->len;
        *inst = found_instr;///??
        return PARSE_INSTR;
    }
    return PARSE_ERR;
}

/*
 * parse_delim: parse an expected delimiter token (e.g., ',')
 * args
 *     ptr: point to the start of string
 *
 * return
 *     PARSE_DELIM: success, move 'ptr' to the first char after token
 *     PARSE_ERR: error, the value of 'ptr' and 'delim' are undefined
 */
parse_t parse_delim(char **ptr, char delim)
{
    /* skip the blank and check */
    //check end???
    SKIP_AND_CHECK(*ptr);
    /* set 'ptr' */
    if(**ptr == delim){
        ++(*ptr);
        return PARSE_DELIM;
    }
    return PARSE_ERR;
}

/*
 * parse_reg: parse an expected register token (e.g., '%rax')
 * args
 *     ptr: point to the start of string
 *     regid: point to the regid of register
 *
 * return
 *     PARSE_REG: success, move 'ptr' to the first char after token, 
 *                         and store the regid to 'regid'
 *     PARSE_ERR: error, the value of 'ptr' and 'regid' are undefined
 */
parse_t parse_reg(char **ptr, regid_t *regid)
{
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    /* find register */
    const reg_t * found_reg = find_register(*ptr);
    
    /* set 'ptr' and 'regid' */
    if(found_reg)
    {
        *ptr += found_reg->namelen;
        *regid = found_reg->id;
        return PARSE_REG;
    }
    return PARSE_ERR;
}

/*
 * parse_symbol: parse an expected symbol token (e.g., 'Main')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *
 * return
 *     PARSE_SYMBOL: success, move 'ptr' to the first char after token,
 *                               and allocate and store name to 'name'
 *     PARSE_ERR: error, the value of 'ptr' and 'name' are undefined
 */
parse_t parse_symbol(char **ptr, char **name)
{
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    /* allocate name and copy to it */
    /* set 'ptr' and 'name' */
    if(IS_LETTER(*ptr)){//symbol should start with letter 
        char* begin_ptr = *ptr;
        int len = 0;
        while(IS_LETTER(*ptr) || IS_DIGIT(*ptr)){
            //constituted of letter or digit 
            ++len;
            ++(*ptr);
        }
        *name = (char *)malloc(sizeof(char) *(len + 1));
        *(*name+len) = '\0';
        strncpy(*name,begin_ptr,len);
        return PARSE_SYMBOL;
    }
    return PARSE_ERR;
}

/*
 * parse_digit: parse an expected digit token (e.g., '0x100')
 * args
 *     ptr: point to the start of string
 *     value: point to the value of digit
 *
 * return
 *     PARSE_DIGIT: success, move 'ptr' to the first char after token
 *                            and store the value of digit to 'value'
 *     PARSE_ERR: error, the value of 'ptr' and 'value' are undefined
 */
parse_t parse_digit(char **ptr, long *value)
{
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    if(!IS_DIGIT(*ptr)) return PARSE_ERR;
    /* calculate the digit, (NOTE: see strtoul()) */
    // if base is 0, the numeric base if auto-detected
    /* set 'ptr' and 'value' */
    *value = strtoul(*ptr,ptr,0);
    return PARSE_DIGIT;
}

/*
 * parse_imm: parse an expected immediate token (e.g., '$0x100' or 'STACK')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *     value: point to the value of digit
 *
 * return
 *     PARSE_DIGIT: success, the immediate token is a digit,
 *                            move 'ptr' to the first char after token,
 *                            and store the value of digit to 'value'
 *     PARSE_SYMBOL: success, the immediate token is a symbol,
 *                            move 'ptr' to the first char after token,
 *                            and allocate and store name to 'name' 
 *     PARSE_ERR: error, the value of 'ptr', 'name' and 'value' are undefined
 */
parse_t parse_imm(char **ptr, char **name, long *value)
{
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    /* if IS_IMM, then parse the digit */
    /* set 'ptr' and 'value' */
    if(IS_IMM(*ptr))
    {
        if(parse_delim(ptr, '$') == -1)
            return PARSE_ERR;
        return parse_digit(ptr,value);
    }
    /* if IS_LETTER, then parse the symbol */
    /* set 'ptr' and 'name' */
    else if(IS_LETTER(*ptr)){
        return parse_symbol(ptr,name);
    }
    return PARSE_ERR;
}

/*
 * parse_mem: parse an expected memory token (e.g., '8(%rbp)')
 * args
 *     ptr: point to the start of string
 *     value: point to the value of digit
 *     regid: point to the regid of register
 * return
 *     PARSE_MEM: success, move 'ptr' to the first char after token,
 *                          and store the value of digit to 'value',
 *                          and store the regid to 'regid'
 *     PARSE_ERR: error, the value of 'ptr', 'value' and 'regid' are undefined
 */
parse_t parse_mem(char **ptr, long *value, regid_t *regid)
{
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    /* calculate the digit and register, (ex: (%rbp) or 8(%rbp)) */
    /* set 'ptr', 'value' and 'regid' */
    parse_digit(ptr,value);
    if(parse_delim(ptr,'(') == PARSE_ERR)
        return PARSE_ERR;
    parse_reg(ptr,regid);
    if(parse_delim(ptr,')') == PARSE_DELIM)
        return PARSE_MEM;
    return PARSE_ERR;
}

/*
 * parse_data: parse an expected data token (e.g., '0x100' or 'array')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *     value: point to the value of digit
 *
 * return
 *     PARSE_DIGIT: success, data token is a digit,
 *                            and move 'ptr' to the first char after token,
 *                            and store the value of digit to 'value'
 *     PARSE_SYMBOL: success, data token is a symbol,
 *                            and move 'ptr' to the first char after token,
 *                            and allocate and store name to 'name' 
 *     PARSE_ERR: error, the value of 'ptr', 'name' and 'value' are undefined
 */
parse_t parse_data(char **ptr, char **name, long *value)
{//check'$'??
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    /* set 'ptr', 'name' and 'value' */
    /* if IS_DIGIT, then parse the digit */
    if(IS_DIGIT(*ptr))
        return parse_digit(ptr,value);
    /* if IS_LETTER, then parse the symbol */
    if(IS_LETTER(*ptr))
        return parse_symbol(ptr,name);
    return PARSE_ERR;    
}

/*
 * parse_label: parse an expected label token (e.g., 'Loop:')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *
 * return
 *     PARSE_LABEL: success, move 'ptr' to the first char after token
 *                            and allocate and store name to 'name'
 *     PARSE_ERR: error, the value of 'ptr' is undefined
 */
parse_t parse_label(char **ptr, char **name)
{
    /* skip the blank and check */
    SKIP_AND_CHECK(*ptr);
    /* allocate name and copy to it */
    /* set 'ptr' and 'name' */
    
    int len = 0;
    char * copied_ptr = *ptr;
    if(IS_LETTER(copied_ptr)){
        ++len;
        ++copied_ptr;
        while( IS_LETTER(copied_ptr) 
            || IS_DIGIT(copied_ptr) || (*copied_ptr == ':'))
        {
            if(*copied_ptr == ':')
            {//有冒号才修改*ptr
                *name = (char *) malloc(sizeof(char) * (len+1));
                *(*name+len) = '\0';
                strncpy(*name,*ptr,len);
                *ptr += (len+1);
                return PARSE_LABEL;
            }
            ++len;
            ++copied_ptr;
        }
    }
    return PARSE_ERR;
}

/*
 * parse_line: parse a line of y64 code (e.g., 'Loop: mrmovq (%rcx), %rsi')
 * (you could combine above parse_xxx functions to do it)
 * args
 *     line: point to a line_t data with a line of y64 assembly code
 *
 * return
 *     TYPE_XXX: success, fill line_t with assembled y64 code
 *     TYPE_ERR: error, try to print err information
 */
type_t parse_line(line_t *line)
{

/* when finish parse an instruction or lable, we still need to continue check 
* e.g., 
*  Loop: mrmovl (%rbp), %rcx
*           call SUM  #invoke SUM function */

    /* skip blank and check IS_END */
    //SKIP_AND_CHECK(line->y64asm);
    char * copied_y64asm_p = line->y64asm;
    SKIP_AND_CHECK(copied_y64asm_p);//QAQ!! bug1
    //protect the char *(y64asm) in "line", so that it will not be changed 
    
    /* is a comment ? */
    char* _label;
    
    if(IS_COMMENT(copied_y64asm_p)){
        line->type = TYPE_COMM;
        return line->type;
    }
    /* is a label ? */
    if(parse_label(&copied_y64asm_p,&_label) == PARSE_LABEL){//this line
        if(add_symbol(_label) == -1){
            err_print("Dup symbol:%s", _label);
            return TYPE_ERR;
        }
        line->type = TYPE_INS;
        (line->y64bin).addr = vmaddr;///有啥用呢？ 有用有用
        (line->y64bin).bytes = 0;//may be changed by an instr followed
    }
    /* is an instruction ? */
    instr_t *_inst_p;
    if(parse_instr(&copied_y64asm_p,&_inst_p) == PARSE_INSTR){
        //set type
        line->type = TYPE_INS;
        //set y64bin
        byte_t byte0 = _inst_p->code;
        (line->y64bin).addr = vmaddr;
        (line->y64bin).codes[0] = byte0;
        (line->y64bin).bytes = _inst_p->bytes;
        
        
    /* update vmaddr */    
        vmaddr += _inst_p->bytes;
        
    /* parse the rest of instruction according to the itype */
        regid_t regA = REG_NONE,regB = REG_NONE;
        long _value = 0;
        char* _name;
        switch(HIGH(byte0))
        {
            case I_HALT:
            case I_NOP:
            case I_RET:
                break;
            case I_RRMOVQ:
            {
                if(parse_reg(&copied_y64asm_p,&regA) == PARSE_ERR)
                {
                    
                    err_print("Invalid REG");
                    line->type = TYPE_ERR;
                    break;
                }
                if(parse_delim(&copied_y64asm_p,',') == PARSE_ERR)
                {
                    err_print("Invalid ','");
                    line->type = TYPE_ERR;
                    break;
                }
                if(parse_reg(&copied_y64asm_p,&regB) == PARSE_ERR)
                {
                    
                    err_print("Invalid REG");
                    line->type = TYPE_ERR;
                    break;
                }
                (line->y64bin).codes[1] = HPACK(regA,regB);
                break;
            }
            case I_IRMOVQ:
            {
                parse_t parse_1 = parse_imm(&copied_y64asm_p,&_name,&_value);
                if(parse_1 == PARSE_ERR){
                    err_print("Invalid Immediate");
                    line->type = TYPE_ERR;
                    break;
                }else if(parse_delim(&copied_y64asm_p,',') == PARSE_ERR){
                    err_print("Invalid ','");
                    line->type = TYPE_ERR;
                    break;
                }else if(parse_reg(&copied_y64asm_p,&regB) == PARSE_ERR){
                    
                    err_print("Invalid REG");
                    line->type = TYPE_ERR;
                    break;
                }
                (line->y64bin).codes[1] = HPACK(regA,regB);
                if(parse_1 == PARSE_SYMBOL)
                {
                    add_reloc(_name,&(line->y64bin));
                }else if(parse_1 == PARSE_DIGIT)
                {//2-9
                    write_bytes_little_endian(_value,
                                            (line->y64bin).codes + 2,8);
                }
                break;
            }
            case I_RMMOVQ:
            {
                if(parse_reg(&copied_y64asm_p,&regA) == PARSE_ERR)
                {
                    err_print("Invalid REG");
                    break;
                }
                if(parse_delim(&copied_y64asm_p,',') == PARSE_ERR)
                {
                    err_print("Invalid ','");
                    break;
                }
                if(parse_mem(&copied_y64asm_p,&_value,&regB) == PARSE_ERR)
                {
                    err_print("Invalid MEM");
                    break;
                }
                (line->y64bin).codes[1] = HPACK(regA,regB);
                write_bytes_little_endian(_value,
                                            (line->y64bin).codes + 2,8);
                break;
            }
            case I_MRMOVQ:
            {
                if(parse_mem(&copied_y64asm_p,&_value,&regB) == PARSE_ERR)
                {
                    err_print("Invalid MEM");
                    break;
                }
                if(parse_delim(&copied_y64asm_p,',') == PARSE_ERR)
                {
                    err_print("Invalid ','");
                    break;
                }
                if(parse_reg(&copied_y64asm_p,&regA) == PARSE_ERR)
                {
                    err_print("Invalid REG");
                    break;
                }
                (line->y64bin).codes[1] = HPACK(regA,regB);
                write_bytes_little_endian(_value,
                                            (line->y64bin).codes + 2,8);
                break;    
            }
            case I_ALU:
            {
                if(parse_reg(&copied_y64asm_p,&regA) == PARSE_ERR)
                {
                    err_print("Invalid REG");
                    break;
                }
                if(parse_delim(&copied_y64asm_p,',') == PARSE_ERR)
                {
                    err_print("Invalid ','");
                    break;
                }
                if(parse_reg(&copied_y64asm_p,&regB) == PARSE_ERR)
                {
                    err_print("Invalid REG");
                    break;
                }
                (line->y64bin).codes[1] = HPACK(regA,regB);
                break;
            }
            case I_JMP:
            case I_CALL:
            {
                if(parse_symbol(&copied_y64asm_p,&_name) == PARSE_ERR)
                {
                    err_print("Invalid DEST");
                    break;
                }
                add_reloc(_name,&(line->y64bin));
                break;
            }
            case I_PUSHQ:
            case I_POPQ:
            {
                if(parse_reg(&copied_y64asm_p,&regA) == PARSE_ERR)
                {
                    err_print("Invalid REG");
                    break;
                }
                (line->y64bin).codes[1] = HPACK(regA,regB);
                break;
            }
            case I_DIRECTIVE:
            {
                switch(LOW(byte0))
                {
                    case D_DATA:
                    {
                        
                        parse_t parse_result = parse_data(&copied_y64asm_p,&_name,&_value);
                        if(parse_result == PARSE_ERR)
                        {
                            err_print("Invalid DATA");
                        }
                        else if(parse_result == PARSE_DIGIT){
                write_bytes_little_endian(_value,
                                            (line->y64bin.codes),_inst_p->bytes);
                        }else {
                add_reloc(_name,&(line->y64bin));
                        }
                break;
                    }
                    case D_POS:
                    {
                        if(parse_digit(&copied_y64asm_p,&_value) == PARSE_ERR)
                        {
                            err_print("Invalid DIGIT");
                            break;
                        }
                        vmaddr = _value;
                        (line->y64bin).addr = vmaddr;
                        break;
                    }
                    case D_ALIGN:
                    {
                        if(parse_digit(&copied_y64asm_p,&_value) == PARSE_ERR)
                        {
                            err_print("Invalid DIGIT");
                            break;
                        }
                        long tmp = vmaddr%_value;
                        if(tmp) vmaddr += _value - tmp;
                        (line->y64bin).addr = vmaddr;
                        break;
                    }
                }
            }
        }
    }
    //汇编语言文件 每行末尾注释可以不处理吧QAQ
    SKIP_BLANK(copied_y64asm_p);
    if(!IS_COMMENT(copied_y64asm_p) && !IS_END(copied_y64asm_p)){
        line->type = TYPE_ERR;
        return TYPE_ERR;
    }
    return line->type;
}

/*
 * assemble: assemble an y64 file (e.g., 'asum.ys')
 * args
 *     in: point to input file (an y64 assembly file)
 *
 * return
 *     0: success, assmble the y64 file to a list of line_t
 *     -1: error, try to print err information (e.g., instr type and line number)
 */
int assemble(FILE *in)
{
    static char asm_buf[MAX_INSLEN]; /* the current line of asm code */
    line_t *line;
    int slen;
    char *y64asm;

    /* read y64 code line-by-line, and parse them to generate raw y64 binary code list */
    while (fgets(asm_buf, MAX_INSLEN, in) != NULL) {
        slen  = strlen(asm_buf);
        while ((asm_buf[slen-1] == '\n') || (asm_buf[slen-1] == '\r')) { 
            asm_buf[--slen] = '\0'; /* replace terminator */
        }

        /* store y64 assembly code */
        y64asm = (char *)malloc(sizeof(char) * (slen + 1)); // free in finit
        strcpy(y64asm, asm_buf);

        line = (line_t *)malloc(sizeof(line_t)); // free in finit
        memset(line, '\0', sizeof(line_t));

        line->type = TYPE_COMM;
        line->y64asm = y64asm;
        line->next = NULL;

        line_tail->next = line;
        line_tail = line;
        lineno ++;

        if (parse_line(line) == TYPE_ERR) {//做parse_line
            return -1;
        }
    }

	lineno = -1;///??
    return 0;
}

/*
 * relocate: relocate the raw y64 binary code with symbol address
 *
 * return
 *     0: success
 *     -1: error, try to print err information (e.g., addr and symbol)
 */
int relocate(void)
{
    
    reloc_t *rtmp = NULL;
    rtmp = reltab->next;
    symbol_t* found_symbol = NULL;
    while (rtmp) {
        /* find symbol */
        if(!(found_symbol = find_symbol(rtmp->name)))
        {
            err_print("Unknown symbol:'%s'",rtmp->name);
            return -1;
        }
        /* relocate y64bin according itype */
                write_bytes_little_endian
                (found_symbol->addr,
                 (rtmp->y64bin->codes) +(rtmp->y64bin->bytes) - 8,
                 8);
        /* next */
        rtmp = rtmp->next;
    }
    return 0;
}

/*
 * binfile: generate the y64 binary file
 * args
 *     out: point to output file (an y64 binary file)
 *
 * return
 *     0: success
 *     -1: error
 */
int binfile(FILE *out)
{
    /* prepare image with y64 binary code */
    line_t * line_p = line_head->next;
    /* binary write y64 code to output file (NOTE: see fwrite()) */
    while(line_p)
    {
        if(line_p->type == TYPE_INS)
        {
            if(fseek(out,(line_p->y64bin).addr,SEEK_SET))
                return -1;
            fwrite((line_p->y64bin).codes,1,(line_p->y64bin).bytes,out);
        }
        line_p = line_p->next;
    }
    
    return 0;
}


/* whether print the readable output to screen or not ? */
bool_t screen = FALSE; 

static void hexstuff(char *dest, int value, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        char c;
        int h = (value >> 4*i) & 0xF;
        c = h < 10 ? h + '0' : h - 10 + 'a';
        dest[len-i-1] = c;
    }
}

void print_line(line_t *line)
{
    char buf[64];

    /* line format: 0xHHH: cccccccccccc | <line> */
    if (line->type == TYPE_INS) {
        bin_t *y64bin = &line->y64bin;
        int i;
        
        strcpy(buf, "  0x000:                      | ");
        
        hexstuff(buf+4, y64bin->addr, 3);
        if (y64bin->bytes > 0)
            for (i = 0; i < y64bin->bytes; i++)
                hexstuff(buf+9+2*i, (y64bin->codes)[i]&0xFF, 2);
    } else {
        strcpy(buf, "                              | ");
    }

    printf("%s%s\n", buf, line->y64asm);
}

/* 
 * print_screen: dump readable binary and assembly code to screen
 * (e.g., Figure 4.8 in ICS book)
 */
void print_screen(void)
{
    line_t *tmp = line_head->next;
    while (tmp != NULL) {
        print_line(tmp);
        tmp = tmp->next;
    }
}

/* init and finit */
void init(void)
{
    reltab = (reloc_t *)malloc(sizeof(reloc_t)); // free in finit
    memset(reltab, 0, sizeof(reloc_t));

    symtab = (symbol_t *)malloc(sizeof(symbol_t)); // free in finit
    memset(symtab, 0, sizeof(symbol_t));

    line_head = (line_t *)malloc(sizeof(line_t)); // free in finit
    memset(line_head, 0, sizeof(line_t));
    line_tail = line_head;
    lineno = 0;
}

void finit(void)
{
    

    
    reloc_t *rtmp = NULL;
    do {
        rtmp = reltab->next;
        if (reltab->name) 
            free(reltab->name);
        free(reltab);
        reltab = rtmp;
    } while (reltab);//free reltab
    
    symbol_t *stmp = NULL;
    do {
        stmp = symtab->next;
        if (symtab->name) 
            free(symtab->name);
        free(symtab);
        symtab = stmp;
    } while (symtab);//free symtab
    
    line_t *ltmp = NULL;
    do {
        ltmp = line_head->next;
        if (line_head->y64asm){
            free(line_head->y64asm);//小朋友你有问题哇
            //我错了 QAQ 我的问题 我的我的
        }
        free(line_head);
        line_head = ltmp;
    } while (line_head);//free line linked_list
    
}

static void usage(char *pname)
{
    printf("Usage: %s [-v] file.ys\n", pname);
    printf("   -v print the readable output to screen\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    int rootlen;
    char infname[512];
    char outfname[512];
    int nextarg = 1;
    FILE *in = NULL, *out = NULL;
    
    if (argc < 2)
        usage(argv[0]);
    
    if (argv[nextarg][0] == '-') {
        char flag = argv[nextarg][1];
        switch (flag) {
          case 'v':
            screen = TRUE;
            nextarg++;
            break;
          default:
            usage(argv[0]);
        }
    }

    /* parse input file name */
    rootlen = strlen(argv[nextarg])-3;
    /* only support the .ys file */
    if (strcmp(argv[nextarg]+rootlen, ".ys"))
        usage(argv[0]);
    
    if (rootlen > 500) {
        err_print("File name too long");
        exit(1);
    }
 

    /* init */
    init();

    
    /* assemble .ys file */
    strncpy(infname, argv[nextarg], rootlen);
    strcpy(infname+rootlen, ".ys");
    in = fopen(infname, "r");
    if (!in) {
        err_print("Can't open input file '%s'", infname);
        exit(1);
    }
    
    if (assemble(in) < 0) {//!!caught you!
        err_print("Assemble y64 code error");
        fclose(in);
        exit(1);
    }
    fclose(in);


    /* relocate binary code */
    if (relocate() < 0) {
        err_print("Relocate binary code error");
        exit(1);
    }


    /* generate .bin file */
    strncpy(outfname, argv[nextarg], rootlen);
    strcpy(outfname+rootlen, ".bin");
    out = fopen(outfname, "wb");
    if (!out) {
        err_print("Can't open output file '%s'", outfname);
        exit(1);
    }

    if (binfile(out) < 0) {
        err_print("Generate binary file error");
        fclose(out);
        exit(1);
    }
    fclose(out);
    
    /* print to screen (.yo file) */
    if (screen)
       print_screen(); 

    /* finit */
    finit();
    return 0;
}


