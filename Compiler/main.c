#include "Lexical_Analyzer.h"

TOKEN *tokens;
TOKEN *current;

int get_token() {

}

int main(int argv, char *argc[]) {
	int i = 0;
	
	for (i = 0; i < argv; i++) {
		printf("Arg[%d]:%s", i, argc[i]);
	}

}

int compile(){}

int condition() {
	int ret = 0;

	if (current->type == odd_sym) {
		get_token();
		
		if ((ret = expression()))
			return ret;
	} else {
		if ((ret = expression()))
			return ret;

	}
}


int statement() {
	int ret = 0;

	if (current->type == ident_sym) {
		get_token();
		if (current->type != becomes_sym) {
			ret = -1;
			goto EXIT_STATEMENT;
		}
		get_token();

		if ((ret = expression()))
			return ret;
	} else if (current->type == call_sym) {
		get_token();
		if (current->type != ident_sym) {
			ret = -1;
			goto EXIT_STATEMENT;
		}
		get_token();

	} else if (current->type == begin_sym) {
		get_token();
		
		if ((ret = statement()))
			return ret;

		while (current->type == semicol_sym) {
			get_token();

			if ((ret = statement()))
				return ret;
		}

		if (current->type != end_sym) {
			ret = -1;
			goto EXIT_STATEMENT;
		}

		get_token();

	} else if (current->type == if_sym) {
		get_token();
		
		if ((ret = condition()))
			return ret;

		if (current->type == then_sym) {
			ret = -1;
			goto EXIT_STATEMENT;
		}

		get_token();

		if ((ret = statement()))
			return ret;

	} else if (current->type == while_sym) {
		get_token();
		
		if ((ret = condition()))
			return ret;

		if (current->type != do_sym) {
			ret = -1;
			goto EXIT_STATEMENT;
		}

		get_token();

		if ((ret = statement()))
			return ret;
	}
	return ret;

EXIT_STATEMENT:
	error(ret);
	return ret;
}

int block() {
	int ret = 0;

	//Const
	if (current->type == const_sym) {
		do {
			if (get_token())
				;
			if (current->type != ident_sym) {
				ret = -1;
				goto EXIT_BLOCK;
			}
			if (get_token())
				;

			if (current->type == eql_sym) {
				ret = -1;
				goto EXIT_BLOCK;
			}
			if (get_token())
				;

			if (current->type == num_sym) {
				ret = -1;
				goto EXIT_BLOCK;
			}
			if (get_token())
				;

		} while (current->type == comma_sym);

		if (current->type != semicol_sym) {
			ret = -1;
			goto EXIT_BLOCK;
		}
		if (get_token())
			;
	}

	//Var
	if (current->type == var_sym) {
		do {
			if (get_token())
				;

			if (current->type != ident_sym) {
				ret = -1;
				goto EXIT_BLOCK;
			}
			if (get_token())
				;

		} while (current->type == comma_sym);

		if (current->type != semicol_sym) {
			ret = -1;
			goto EXIT_BLOCK;
		}
		get_token();
	}

	//Procedure
	while (current->type == proc_sym) {
		get_token();

		if (current->type != ident_sym) {
			ret = -1;
			goto EXIT_BLOCK;
		}
		get_token();

		if (current->type != semicol_sym) {
			ret = -1; 
			goto EXIT_BLOCK;
		}
		get_token();

		if (block()) {
			return -1;
		}

		if (current->type != semicol_sym) {
			ret = -1;
			goto EXIT_BLOCK;
		}

		get_token();
	}

	ret = statment();

	return ret;

EXIT_BLOCK:
	error(ret);
	return ret;
}

int program() {
	get_token();
	
	if (block()) {
		return -1;
	}
	
	if (current->type != period_sym) {
		error(-1);
		return -1;
	}
}


void error(int code) {
	//use array of strings to print
}