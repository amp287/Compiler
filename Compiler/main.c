//Austin Peace
//Sebastian De La Cruz
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Lexical_Analyzer.h"
#include "VM.h"
#include "Errors.h"

#define MAX_TABLE_SIZE 10

typedef struct {
    int kind;		//const = const_sym, var = var_sym, proc = proc_sym
    char name[12];	//name
    int val;		//number value
    int level;		//L level
    int addr;		//M address
}SYMBOL;

SYMBOL symbol_table[MAX_TABLE_SIZE];
int symbols_in_table;

static TOKEN *start;
static TOKEN *token;
static INSTRUCTION code[MAX_CODE_LENGTH];

int error_flag;
int code_index;
int reg;
int lex_level;
int m_addr;

int program();
int expression();
int statement();

static void print_instructions(int print_flag) {
    int i;
    FILE *fp;

    fp = fopen("Assembly.txt", "w");

    for (i = 0; i < code_index; i++) {
        printf("%s %d %d %d\n", opcodes[code[i].op], code[i].r, code[i].l, code[i].m);
        fprintf(fp, "%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
    }

    fclose(fp);
}

void error(int code) {
    printf("ERROR: %s\n", errors[code]);
}

int add_symbol(TOKEN *token, int value, int type) {
    if (symbols_in_table > MAX_TABLE_SIZE)
        return -1;
    symbol_table[symbols_in_table].kind = type;
    strcpy(symbol_table[symbols_in_table].name, token->value);
    symbol_table[symbols_in_table].val = value;
    symbol_table[symbols_in_table].level = lex_level;
    symbol_table[symbols_in_table].addr = m_addr;
    symbols_in_table++;
    return 0;
}

//return index
//-1 on fail
int check_ident(TOKEN *token) {
    int i;

    for (i = 0; i < symbols_in_table; i++) {
        if (!strcmp(token->value, symbol_table[i].name)) {
            return i;
        }
    }
    return -1;
}

int emit(int op, int r, int l, int m) {
    if (code_index > MAX_CODE_LENGTH)
        return 29;

    code[code_index].op = op;
    code[code_index].r = r;
    code[code_index].l = l;
    code[code_index].m = m;
    code_index++;

    return 0;
}

static int get_token() {
    if(token == NULL){
        token = start;
        return 0;
    }

    if (token->next == NULL) {
        error_flag = 26;
        return 26;
    }

    token = token->next;

    return 0;
}

int main(int argv, char *argc[]) {
    int i = 0, vm_flag = 0, lex_flag = 0, assembly_flag = 0, file_index = -1;
    token = NULL;
    start = NULL;
    error_flag = 0;
    symbols_in_table = 0;
    reg = 0;
    m_addr = 0;
    lex_level = 0;

    for (i = 1; i < argv; i++) {
        if (!strcmp(argc[i], "-l"))
            lex_flag = 1;
        else if (!strcmp(argc[i], "-a"))
            assembly_flag = 1;
        else if (!strcmp(argc[i], "-v"))
            vm_flag = 1;
        else if (!strcmp(argc[i], "-f"))
            if (argv > i + 1)
                file_index = i + 1;
    }

    if ((start = run_lexical_analyzer(argc[file_index], lex_flag)) == NULL) {
        printf("Lexigraphical Analyzer has encountered an error...\n");
        getchar();
        return 0;
    }

    if (program()) {
        printf("Parser has encountered an error...\n");
        return(1);
    }

    printf("\nNo errors, program is syntactically correct\n");

    if (assembly_flag)
        printf("\nInstructions\n");

    print_instructions(assembly_flag);

    run_VM("Assembly.txt", vm_flag);

    return 0;
}

int is_relation(int token) {
    switch (token) {
        case eql_sym:
            return EQL;
        case neq_sym:
            return NEQ;
        case less_sym:
            return LSS;
        case leq_sym:
            return LEQ;
        case gtr_sym:
            return GTR;
        case geq_sym:
            return GEQ;
        default:
            return 0;
    }
}

int factor() {
    int ret = 0;
    int index = -1;

    if (token->type == ident_sym) {

        if ((index = check_ident(token)) == -1) {
            error(11);
            return 11;
        }

        if (symbol_table[index].kind == proc_sym) {		//check for const or var
            error(23);
            return 23;
        }

        emit(LOD, ++reg, symbol_table[index].level, symbol_table[index].addr);

        if (get_token()) goto EXIT_FACTOR;

    } else if (token->type == num_sym) {

        emit(LIT, ++reg, 0, atoi(token->value));

        if (get_token()) goto EXIT_FACTOR;

    } else if (token->type == lparent_sym) {

        if (get_token()) goto EXIT_FACTOR;

        if ((ret = expression()))
            return ret;

        if (token->type != rparent_sym) {
            error(22);
            return 22;
        }

        if (get_token()) goto EXIT_FACTOR;

    } else {
        error(23);
        return 23;
    }

    return ret;

    EXIT_FACTOR:
    error(error_flag);
    return error_flag;
}

int term() {
    int ret = 0;
    int mult = 0;

    if ((ret = factor()))
        return ret;

    while (token->type == mult_sym || token->type == slash_sym) {

        if (token->type == mult_sym)
            mult = 1;

        if (get_token()) goto EXIT_TERM;

        if ((ret = factor()))
            return ret;

        if (mult)
            emit(MUL, reg - 1, reg - 1, reg);
        else
            emit(DIV, reg - 1, reg - 1, reg);
        reg--;
    }

    return ret;

    EXIT_TERM:
    error(error_flag);
    return error_flag;
}

int expression() {
    int ret = 0;
    int plus = 0;

    if (token->type == plus_sym || token->type == minus_sym) {

        if (token->type == plus_sym)
            plus = 1;

        if (get_token()) goto EXIT_EXPRESSION;

        if ((ret = term()))
            return ret;

        if (plus)
            emit(ADD, reg - 1, reg - 1, reg);
        else
            emit(SUB, reg - 1, reg - 1, reg);
        reg--;
    } else {

        if ((ret = term()))
            return ret;
    }

    while (token->type == plus_sym || token->type == minus_sym) {

        if (token->type == plus_sym)
            plus = 1;

        if (get_token()) goto EXIT_EXPRESSION;

        if ((ret = term()))
            return ret;

        if (plus)
            emit(ADD, reg - 1, reg - 1, reg);
        else
            emit(SUB, reg - 1, reg - 1, reg);
        reg--;
    }
    return ret;

    EXIT_EXPRESSION:
    error(error_flag);
    return error_flag;
}

int condition() {
    int ret = 0;
    int op = 0;

    if (token->type == odd_sym) {

        if (get_token()) goto EXIT_CONDITION;

        if ((ret = expression()))
            return ret;

        emit(ODD, reg, 0, 0);

    } else {
        if ((ret = expression()))
            return ret;

        if (!(op = is_relation(token->type))) {
            error(20);
            return 20;
        }

        if (get_token()) goto EXIT_CONDITION;

        if ((ret = expression()))
            return ret;

        emit(op, reg - 1, reg - 1, reg);
        reg--;
    }

    return 0;

    EXIT_CONDITION:
    error(error_flag);
    return error_flag;
}

int statement() {
    int ret = 0;
    int index = -1;
    int code_temp, code_temp2;

    if (token->type == ident_sym) {

        if ((index = check_ident(token)) == -1) {
            error(11);
            return 11;
        }

        if (symbol_table[index].kind != var_sym) {
            error(12);
            return 12;
        }

        if (get_token()) goto EXIT_STATEMENT;

        if (token->type != becomes_sym) {
            error(27);
            return 27;
        }

        if (get_token()) goto EXIT_STATEMENT;

        if ((ret = expression()))
            return ret;

        emit(STO, reg, symbol_table[index].level, symbol_table[index].addr);
        reg--;

    } else if (token->type == call_sym) {

        if (get_token()) goto EXIT_STATEMENT;

        if (token->type != ident_sym) {
            error(14);
            return 14;
        }

        if ((index = check_ident(token)) == -1) {
            error(11);
            return 11;
        }

        if (token->type != proc_sym) {
            error(15);
            return 15;
        }

        if (get_token()) goto EXIT_STATEMENT;

    } else if (token->type == begin_sym) {

        if (get_token()) goto EXIT_STATEMENT;

        if ((ret = statement()))
            return ret;

        while (token->type == semicol_sym) {

            if (get_token()) goto EXIT_STATEMENT;

            if ((ret = statement()))
                return ret;
        }

        if (token->type != end_sym) {
            error(8);
            return 8;
        }

        if (get_token()) goto EXIT_STATEMENT;

    } else if (token->type == if_sym) {

        if (get_token()) goto EXIT_STATEMENT;

        if ((ret = condition()))
            return ret;

        if (token->type != then_sym) {
            error(16);
            return 16;
        }

        if (get_token()) goto EXIT_STATEMENT;

        code_temp = code_index;

        emit(JPC, reg, 0, 0);
        reg--;

        if ((ret = statement()))
            return ret;

        code[code_temp].m = code_index;

    } else if (token->type == while_sym) {

        code_temp = code_index;

        if (get_token()) goto EXIT_STATEMENT;

        if ((ret = condition()))
            return ret;

        code_temp2 = code_index;

        emit(JPC, reg, 0, 0);
        reg--;

        if (token->type != do_sym) {
            error(18);
            return 18;
        }

        if (get_token()) goto EXIT_STATEMENT;

        if ((ret = statement()))
            return ret;

        emit(JMP, 0, 0, code_temp);
        code[code_temp2].m = code_index;
    }
    return ret;

    EXIT_STATEMENT:
    error(error_flag);
    return error_flag;
}

int block() {
    int ret = 0;
    TOKEN *ident;
    //Const
    if (token->type == const_sym) {
        do {

            if (get_token()) goto EXIT_BLOCK;

            if (token->type != ident_sym) {
                error(4);
                return 4;
            }

            ident = token;

            if (get_token()) goto EXIT_BLOCK;

            if (token->type != eql_sym) {
                error(3);
                return 3;
            }

            if (get_token()) goto EXIT_BLOCK;

            if (token->type != num_sym) {
                error(2);
                return 2;
            }

            if (add_symbol(ident, atoi(token->value), const_sym)) {
                error(28);
                return 28;
            }

            emit(INC, 0, 0, 1);

            m_addr++;

            if (get_token()) goto EXIT_BLOCK;

        } while (token->type == comma_sym);

        if (token->type != semicol_sym) {
            error(5);
            return 5;
        }

        if (get_token()) goto EXIT_BLOCK;
    }

    //Var
    if (token->type == var_sym) {
        do {

            if (get_token()) goto EXIT_BLOCK;

            if (token->type != ident_sym) {
                error(4);
                return 4;
            }

            if (add_symbol(token, 0, var_sym)) {
                error(28);
                return 28;
            }

            emit(INC, 0, 0, 1);

            m_addr++;

            if (get_token()) goto EXIT_BLOCK;

        } while (token->type == comma_sym);

        if (token->type != semicol_sym) {
            error(5);
            return 5;
        }

        if (get_token()) goto EXIT_BLOCK;
    }

    //Procedure
    while (token->type == proc_sym) {

        if (get_token()) goto EXIT_BLOCK;

        if (token->type != ident_sym) {
            error(4);
            return 4;
        }

        if (add_symbol(token, 0, proc_sym)) {
            error(28);
            return 28;
        }

        if (get_token()) goto EXIT_BLOCK;

        if (token->type != semicol_sym) {
            error(5);
            return 5;
        }

        if (get_token()) goto EXIT_BLOCK;

        if (block()) {
            return -1;
        }

        if (token->type != semicol_sym) {
            error(5);
            return 5;
        }

        if (get_token()) goto EXIT_BLOCK;
    }

    ret = statement();

    return ret;

    EXIT_BLOCK:
    error(error_flag);
    return error_flag;
}

int program() {
    int ret = 0;

    if (get_token()) {
        error(error_flag);
        return error_flag;
    }

    if ((ret = block())) {
        return ret;
    }

    if (token->type != period_sym) {
        error(9);
        return 9;
    }

    emit(SIO, 0, 0, 3);

    return ret;
}