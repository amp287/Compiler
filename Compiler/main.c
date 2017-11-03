#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Lexical_Analyzer.h"
#include "VM.h"
#include "Errors.h"

static TOKEN *start;
static TOKEN *token;

int error_flag;

int program();
int expression();
int statement();

void error(int code) {
	printf("ERROR: %s\n", errors[code]);
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
	}

	getchar();
	return 0;
}


int is_relation(int token) {
	switch (token) {
		case eql_sym:
		case neq_sym:
		case less_sym:
		case leq_sym:
		case gtr_sym:
		case geq_sym:
			return token;
			break;
		default:
			return 0;
	}
}

int factor() {
	int ret = 0;

	if (token->type == ident_sym) {

		if (get_token()) goto EXIT_FACTOR;

	} else if (token->type == num_sym) {
		
		if (get_token()) goto EXIT_FACTOR;

	} else if (token->type == lparent_sym) {

		if (get_token()) goto EXIT_FACTOR;

		if ((ret = expression()))
			return ret;

		if (token->type != rparent_sym) {
			error_flag = 22;
			goto EXIT_FACTOR;
		}

		if (get_token()) goto EXIT_FACTOR;

	} else {
		error_flag = 23;
		goto EXIT_FACTOR;
	}

	return ret;

EXIT_FACTOR:
	error(error_flag);
	return error_flag;
}

int term() {
	int ret = 0;

	if ((ret = factor()))
		return ret;

	while (token->type == mult_sym || token->type == slash_sym) {

		if (get_token()) goto EXIT_TERM;

		if ((ret = factor()))
			return ret;
	}

	return ret;

EXIT_TERM:
	error(error_flag);
	return error_flag;
}

int expression() {
	int ret = 0;

	if (token->type == plus_sym || token->type == minus_sym)
		if (get_token()) goto EXIT_EXPRESSION;

	if ((ret = term()))
		return ret;

	while (token->type == plus_sym || token->type == minus_sym) {
		
		if (get_token()) goto EXIT_EXPRESSION;

		if ((ret = term()))
			return ret;
	}
	return ret;

EXIT_EXPRESSION:
	error(error_flag);
	return error_flag;
}

int condition() {
	int ret = 0;

	if (token->type == odd_sym) {

		if (get_token()) goto EXIT_CONDITION;
		
		if ((ret = expression()))
			return ret;
	} else {
		if ((ret = expression()))
			return ret;

		if (!is_relation(token->type)) {
			error_flag = 20;
			goto EXIT_CONDITION;
		}

		if (get_token()) goto EXIT_CONDITION;

		if ((ret = expression()))
			return ret;
	}

	return ret;

EXIT_CONDITION:
	error(error_flag);
	return error_flag;
}

int statement() {
	int ret = 0;

	if (token->type == ident_sym) {
		
		if (get_token()) goto EXIT_STATEMENT;

		if (token->type != becomes_sym) {
			error_flag = 27;
			goto EXIT_STATEMENT;
		}

		if (get_token()) goto EXIT_STATEMENT;

		if ((ret = expression()))
			return ret;

	} else if (token->type == call_sym) {

		if (get_token()) goto EXIT_STATEMENT;

		if (token->type != ident_sym) {
			error_flag = 14;
			goto EXIT_STATEMENT;
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
			error_flag = 8;//Maybe?
			goto EXIT_STATEMENT;
		}

		if (get_token()) goto EXIT_STATEMENT;

	} else if (token->type == if_sym) {

		if (get_token()) goto EXIT_STATEMENT;
		
		if ((ret = condition()))
			return ret;

		if (token->type != then_sym) {
			error_flag = 16;
			goto EXIT_STATEMENT;
		}

		if (get_token()) goto EXIT_STATEMENT;

		if ((ret = statement()))
			return ret;

	} else if (token->type == while_sym) {

		if (get_token()) goto EXIT_STATEMENT;
		
		if ((ret = condition()))
			return ret;

		if (token->type != do_sym) {
			error_flag = 18;
			goto EXIT_STATEMENT;
		}

		if (get_token()) goto EXIT_STATEMENT;

		if ((ret = statement()))
			return ret;
	}
	return ret;

EXIT_STATEMENT:
	error(error_flag);
	return error_flag;
}

int block() {
	int ret = 0;

	//Const
	if (token->type == const_sym) {
		do {

			if (get_token()) goto EXIT_BLOCK;

			if (token->type != ident_sym) {
				error_flag = 4;
				goto EXIT_BLOCK;
			}

			if (get_token()) goto EXIT_BLOCK;

			if (token->type != eql_sym) {
				error_flag = 3;
				goto EXIT_BLOCK;
			}

			if (get_token()) goto EXIT_BLOCK;

			if (token->type != num_sym) {
				error_flag = 2;
				goto EXIT_BLOCK;
			}

			if (get_token()) goto EXIT_BLOCK;

		} while (token->type == comma_sym);

		if (token->type != semicol_sym) {
			error_flag = 5;
			goto EXIT_BLOCK;
		}

		if (get_token()) goto EXIT_BLOCK;
	}

	//Var
	if (token->type == var_sym) {
		do {

			if (get_token()) goto EXIT_BLOCK;

			if (token->type != ident_sym) {
				error_flag = 4;
				goto EXIT_BLOCK;
			}

			if (get_token()) goto EXIT_BLOCK;

		} while (token->type == comma_sym);

		if (token->type != semicol_sym) {
			error_flag = 5;
			goto EXIT_BLOCK;
		}

		if (get_token()) goto EXIT_BLOCK;
	}

	//Procedure
	while (token->type == proc_sym) {
		if (get_token()) goto EXIT_BLOCK;

		if (token->type != ident_sym) {
			error_flag = 4;
			goto EXIT_BLOCK;
		}

		if (get_token()) goto EXIT_BLOCK;

		if (token->type != semicol_sym) {
			error_flag = 5; 
			goto EXIT_BLOCK;
		}

		if (get_token()) goto EXIT_BLOCK;

		if (block()) {
			return -1;
		}

		if (token->type != semicol_sym) {
			error_flag = 5;
			goto EXIT_BLOCK;
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
	
	if ((ret =block())) {
		return ret;
	}
	
	if (token->type != period_sym) {
		error(9);
		return 9;
	}

	return ret;
}
