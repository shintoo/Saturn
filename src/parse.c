#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

#define __instruction_count 25

int __linecount = 0;
extern void (*instructions[__instruction_count])(Arg *, const Arg *);
extern int __instruction_location;
extern FILE *src_file;
extern char StatusWord;
Environment *env;

const char *__COMMANDS2  =
	"MOV CAT ADD SUB MUL DIV MOD"
	"OUT RIN FIL OPN CLS CMP";
const char *__COMMANDS1  =
	"INC DEC JMP JEQ JNE JIG JGE JIL JLE";
const char *__COMMANDS12 =
	"INT STR FLT ";

void Init(void) {
	DEBUGMSG("[" _GREEN "  ENV  " _RESET "] Initializing Saturn environment\n");

	env = malloc(sizeof(Environment));
	env->vars = malloc(10 * sizeof(Var *));
	env->memsize = 10;
	env->varcount = 4;

	instructions[0] =  saturn_int;
	instructions[1] =  saturn_flt;
	instructions[2] =  saturn_str;

	instructions[3] =  saturn_add;
	instructions[4] =  saturn_sub;
	instructions[5] =  saturn_mul;
	instructions[6] =  saturn_div;
	instructions[7] =  saturn_mod;

	instructions[8] =  saturn_inc;
	instructions[9] =  saturn_dec;

	instructions[10] =  saturn_mov;
	instructions[11] = saturn_cat;

	instructions[12] = saturn_rin;
	instructions[13] = saturn_out;
	instructions[14] = saturn_fil;
	instructions[15] = saturn_opn;
	instructions[16] = saturn_cls;

	instructions[17] = saturn_jmp;
	instructions[18] = saturn_cmp;
	instructions[19] = saturn_jeq;
	instructions[20] = saturn_jne;
	instructions[21] = saturn_jig;
	instructions[22] = saturn_jge;
	instructions[23] = saturn_jil;
	instructions[24] = saturn_jle;

	env->vars[0] = malloc(sizeof(Var));
	env->vars[0]->label = "stdin";
	env->vars[0]->type = _FIL;
	env->vars[0]->val.FIL.pntr = stdin;
	env->vars[0]->val.FIL.isopen = true;

	env->vars[1] = malloc(sizeof(Var));
	env->vars[1]->val.FIL.pntr = stdout;
	env->vars[1]->type = _FIL;
	env->vars[1]->label = "stdout";
	env->vars[1]->val.FIL.isopen = true;

	env->vars[2] = malloc(sizeof(Var));
	env->vars[2]->label = "stderr";
	env->vars[2]->type = _FIL;
	env->vars[2]->val.FIL.pntr = stderr;
	env->vars[2]->val.FIL.isopen = true;

	env->vars[3] = malloc(sizeof(Var));
	env->vars[3]->label = "newline";
	env->vars[3]->type = _STR;
	env->vars[3]->val.STR = "\n";
}

void End(void) {
	DEBUGMSG("[" _GREEN "  ENV  " _RESET "] Closing Saturn environment\n");

	for (int i = 0; i < 4; i++) {
		free(env->vars[i]);
	}
	for (int i = 4; i < env->varcount; i++) {
		free(env->vars[i]->label);
		if (env->vars[i]->type == _STR) {
			free(env->vars[i]->val.STR);
		}
		if (env->vars[i]->type == _FIL) {
			free(env->vars[i]->val.FIL.path);
		}
		free(env->vars[i]);
	}
	free(env->vars);
	free(env);

	exit(EXIT_SUCCESS);
}
int CountLines(FILE *src_file) {
	char ch;
	int ret = 0;
	do {
		ch = fgetc(src_file);
		if (ch == '\n') {
			ret++;
		}
	} while (!feof(src_file));

	rewind(src_file);
	return ret;
}

char * getline(FILE *src_file) {
	char *ret = malloc(81);
	fgets(ret, 80, src_file);
	replace(ret, ';', '\n');
	__instruction_location++;

	return ret;
}

Statement * Parse(char *line) {
	if (strchr(line, ':') && !strchr(line, '\'')) {
		return NULL;
	}

	Statement *ret;
	char *token;
	char *strlit = NULL; /* */
	char *line_copy; /* line is broken with strtok, this holds a copy */
	char *tab;
	char *comma = ",";
	char *nl = "\n";
	bool onearg = false;
	char *commands[] = {
		"INT", "FLT", "STR", "ADD", "SUB", "MUL", "DIV", "MOD",
		"INC", "DEC", "MOV", "CAT", "RIN", "OUT", "FIL", "OPN",
		"CLS", "JMP", "CMP", "JEQ", "JNE", "JIG", "JGE", "JIL",
		"JLE"
	};
	int temp;

	line_copy = malloc(strlen(line) + 1);
	strcpy(line_copy, line);

	while ((tab = strchr(line, '\t')) != NULL) {
		*tab = ' ';
	}
	
	token = strtok(line, " ");
	if (token[0] == '\n' || token[0] == ';') {
		free(line_copy);
		return NULL;
	}
	ret = NewStatement();
	ToUpper(token);

	/* Check the COMMANDS enum in types.h */
	ret->command = arraystr(commands, __instruction_count, token);

	if (ret->command == -1) {
		ABORT("Error: unknown keyword: %s", token);
	}

	token = strtok(NULL, " ");
	if (!token) {                     // 0 arguments
		ret->argcount = 0;
		free(line_copy);
		return ret;
	}

	comma = strchr(token, ',');       // check if token exists as var
	if (!comma) {
		onearg = true;
		nl = strchr(token, '\n');
		if (nl) {
			*nl = '\0';
		}
		nl = NULL;
	}
	else {
		*comma = '\0';
	}

	ret->args[0] = CreateArg(token);

	token = strtok(NULL, " ");
	if (!token) {                    // 1 argument
		ret->argcount = 1;
		ret->args[1] = NULL;
		free(line_copy);
		return ret;
	}
	if (token[0] == ';') {
		ret->argcount = 1;
		ret->args[1] = NULL;
		free(line_copy);
		return ret;
	}

	if (onearg) {
		ABORT("Missing comma after first argument");
	}
	
	nl = strchr(token, '\n');
	if (nl) {
		*nl = '\0';
	}

	ret->argcount = 2;              // 2 arguments

	/* string literals must be handled differently - tokens are seperated
	 * by spaces, and a string may contain spaces, so the whole line
	 * must be used for string literals
	 */
	if (token[0] == '\'') {
		DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] %d: String literal\n",
		    __LINE__);
		strlit = malloc(80);
		DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Copying literal from line: \"%s\"\n", line_copy);
		strncpy(strlit, strstr(line_copy, token), 79);
		*(strchr(strlit + 1, '\'') + 1) = '\0';
	}
	free(line_copy);
	if (strlit) {
		ret->args[1] = CreateArg(strlit);
	} else {
		ret->args[1] = CreateArg(token);
	}
	return ret;
}

Arg * CreateArg(char *token) {
	Arg * newarg;

	if (token[0] == '\'') {
		newarg = CreateStringLiteral(token);
		return newarg;
	}
	if (isdigit(token[0])) {
		newarg = CreateNumericLiteral(token);
		return newarg;
	}
	if (strchr("abcdefghijklmnopqrstuvwxyz_", tolower(token[0])) == NULL) {
		  ABORT("Illegal character beginning argument: %s", token);
	}

	newarg = CreateVarArg(token);
	return newarg;
}

Arg * CreateStringLiteral(char *token) {
	char *end;
	Arg *ret;
	DEBUGMSG("[ " _MAGENTA "PARSE " _RESET "] %d: token: \"%s\"\n", __LINE__, token);

	if (token[strlen(token) - 1] != '\'') {
		if (strchr(token + 1, '\'') == NULL) {
			ABORT("Missing terminating \' for %s", token);
		}
		ABORT("\' must end string literal: %s", token);
	}
	for (int i = 0; token[i] != '\0'; i++) {
		token[i] = token[i + 1];
	}

	end = strchr(token, '\'');
	*end = '\0';
	
	ret = malloc(sizeof(Arg));
	ret->isliteral = true;
	ret->var = malloc(sizeof(Var));
	ret->var->label = "STR_LITERAL";
	ret->var->type = _STR;
	ret->var->val.STR = token;
	ret->token = token;

	return ret;
}

Arg * CreateNumericLiteral(char *token) {
	bool flt = false;
	Arg *ret;

	for (int i = 0; token[i] != '\0'; i++) {
		if (token[i] == '.') {
			flt = true;
			continue;
		}
		if (!isdigit(token[i])) {
			ABORT("Numeric literals may only contain digits and a decimal: %s",
			       token);
		}
	}

	ret = malloc(sizeof(Arg));
	ret->isliteral = true;
	ret->var = malloc(sizeof(Var));
	if (flt) {
		ret->var->label = "FLT_LITERAL";
		ret->var->type = _FLT;
		ret->var->val.FLT = atof(token);
	} else {
		ret->var->label = "INT_LITERAL";
		ret->var->type = _INT;
		ret->var->val.INT = atoi(token);
	}
	ret->token = token;

	return ret;
}

Arg * CreateVarArg(char *token) {
	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Creating variable argument for \"%s\"\n",
	    token);
	Arg *ret;
	int len = strlen(token);

	for (int i = 0; i < len; i++) {
		if (!isalnum(token[i]) && token[i] != '_') {
			ABORT("Illegal character in variable name: %s", token);
		}
	}
	
	ret = malloc(sizeof(Arg));

	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Searching environment for \"%s\"\n", token);
	if ((ret->var = Env(token)) != NULL) {
		DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Found %s in environment\n",
		    ret->var->label);
		ret->isliteral = false;
		ret->token = token;
		return ret;
	}
	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] \"%s\" not found in environment\n", token);

	ret->isliteral = false;
	ret->token = token;
	return ret;
}

Var * Env(char *token) {
	Var *ret = NULL;

	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Environment contains %d variables\n",
	    env->varcount);
	for (int i = 0; i < env->varcount; i++) {
		DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] \t%s variable: \"%s\"\n",
		    TypeLabel(env->vars[i]->type),
		       env->vars[i]->label);
		if (strcmp(env->vars[i]->label, token) == 0) {
			DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Found %s in environment at %d\n",
				env->vars[i]->label, i);
			ret = env->vars[i];
		}
	}
	
	return ret;
}

Statement * NewStatement(void) {
	Statement *newst = malloc(sizeof(Statement));
	newst->args = malloc(2 * sizeof(Arg *));
	newst->args[0] = NULL;
	newst->args[1] = NULL;
	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Creating Statement at %p\n", newst);
	return newst;
}

void Validate(const Statement *st) {
/*	if (st->argcount > 0) {
		if (st->args[0]->isliteral) {
			ABORT("First argument may not be a literal: %s", st->args[0]->var->label);
		}
	}
*/}

void DeleteStatement(Statement *st) {
	for (int i = 0; i < st->argcount; i++) {
		if (st->args[i]->isliteral) {
			if (st->args[i]->var->type == _STR) {
				free(st->args[i]->var->val.STR);
			}
			DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Freeing literal: \"%s\"\n",
				st->args[i]->var->label);
			free(st->args[i]->var);
		}
		free(st->args[i]);
	}
	free(st->args);
	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Freeing Memory at %p\n", st);
	free(st);
}	

const char * TypeLabel(enum types type) {
	const char *TYPELABELS[] = {
		"INT", "FLT", "STR", "FIL"
	};
	return TYPELABELS[type];
}

void ToUpper(char *st) {
	int len = strlen(st);

	for (int i = 0; i < len; i++) {
		st[i] = toupper(st[i]);
	}
}

int FindLabel(const char *label, fpos_t *loc) {
	DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Searching for \"%s\"\n", label);
	char str[80];
	char *lab;
	char ch;
	rewind(src_file);

	while (!feof(src_file)) {
		fgets(str, 80, src_file);
		for (int i = 0; i < strlen(str); i++) {
			if (!isspace(str[i])) {
				lab = str + i;
				break;
			}
		}
		DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ]\t%s", str);
		if (strstr(lab, label) && lab[0] == label[0]) {
			DEBUGMSG("[ " _MAGENTA "PARSE" _RESET " ] Found label in FindLabel\n");
			fgetpos(src_file, loc);
			return 1;
		}
	}
	ABORT("Label not found: \"%s\"\n", label);
}
