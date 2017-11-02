//Austin Peace
//Sebastian De La Cruz
#define IDENT_MAX_LENGTH 11
#define NUM_MAX_LENGTH 5
#define NUM_SYMBOLS 16
#define NUM_RESERVED 13

typedef struct TOKEN {
	int type;
	char value[10];
	struct TOKEN *next;
}TOKEN;

int run_lexical_analyzer(char *filename, int print_flag, TOKEN **tokens);

void free_tokens(TOKEN *tokens);

enum lex {
	nul_sym = 1, ident_sym, num_sym, plus_sym, minus_sym, mult_sym, slash_sym, odd_sym, eql_sym, neq_sym, less_sym, leq_sym,
	gtr_sym, geq_sym, lparent_sym, rparent_sym, comma_sym, semicol_sym, period_sym, becomes_sym, begin_sym, end_sym, if_sym,
	then_sym, while_sym, do_sym, call_sym, const_sym, var_sym, proc_sym, write_sym, read_sym, else_sym
};


char *reserved[] = { "const","var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};
int reserved_lex[] = { const_sym, var_sym, proc_sym, call_sym, begin_sym, end_sym, if_sym, then_sym, else_sym, while_sym, do_sym,
	read_sym, write_sym, odd_sym};

char *symbols[] = { "+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":=", "<=", ">=", "<>" };
int symbol_lex[] = { plus_sym, minus_sym, mult_sym, slash_sym, rparent_sym, lparent_sym, eql_sym, comma_sym, period_sym, less_sym,
gtr_sym, semicol_sym, becomes_sym, leq_sym, geq_sym, neq_sym };
