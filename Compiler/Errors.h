#define MAX_ERRORS 28
char *errors[] = {
	"NULL",
	"Use = instead of :=.",
	"= must be followed by a number.",
	"Identifier must be followed by =.",
	"const, var, procedure must be followed by identifier.",
	"Semicolon or comma missing.",
	"Incorrect symbol after procedure declaration.",
	"Statement expected.",
	"Incorrect symbol after statement part in block.",
	"Period expected.",
	"Semicolon between statements missing.",
	"Undeclared identifier.",
	"Assignment to constant or procedure is not allowed.",
	"Assignment operator expected.",
	"Call must be followed by an identifier.",
	"Call of a constant or variable is meaningless.",
	"then expected.",
	"Semicolon or } expected.",
	"do expected.",
	"Incorrect symbol following statement.",
	"Relational operator expected.",
	"Expression must not contain a procedure identifier.",
	"Right parenthesis missing.",
	"The preceding factor cannot begin with this symbol.",
	"An expression cannot begin with this symbol.",
	"This number is too large.",
	"Unexpected end of file.",
	":= expected"
};