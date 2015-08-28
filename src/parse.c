#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

#define __instruction_count 17

int __linecount = 0;
extern void (*instructions[__instruction_count])(Arg *, const Arg *);
extern int __instruction_location;

Environment *env;

const char *__COMMANDS2  =
	"MOV CAT ADD SUB MUL DIV MOD"
	"OUT RIN FIL OPN CLS";
const char *__COMMANDS1  =
	"INC DEC ";
const char *__COMMANDS12 =
	"INT STR FLT ";

void Init(void) {
	DEBUGMSG("[ENVIRONMENT] Initializing Saturn environment\n");

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

	env->vars[0] = malloc(sizeof(Var));
	env->vars[0]->label = "stdin";
	env->vars[0]->type = _FIL;
	env->vars[0]->val.FIL.pntr = stdin;
	env->vars[0]->val.FIL.isopen = true;

	env->vars[1] = malloc(sizeof(Var));
	env->vars[1]->label = "stdout";
	env->vars[1]->type = _FIL;
	env->vars[1]->val.FIL.pntr = stdout;
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
	env->vars[3]->val.FIL.isopen = true;
}



void End(void) {
	DEBUGMSG("[ENVIRONMENT] Closing Saturn environment\n");

	for (int i = 0; i < 4; i++) {
		free(env->vars[i]);
	}
	for (int i = 4; i < env->varcount; i++) {
		if (env->vars[i]->type == _STR) {
			free(env->vars[i]->val.STR);
		}
		if (env->vars[i]->type == _FIL) {
			if (env->vars[i]->val.FIL.isopen) {
				fclose(env->vars[i]->val.FIL.pntr);
			}
		}
		free(env->vars[i]);
	}
	free(env->vars);
	free(env);

	exit(EXIT_SUCCESS);
}

int CountLines(FILE *src) {
	char ch;
	int ret = 0;

	do {
		ch = fgetc(src);
		if (ch == '\n') {
			ret++;
		}
	} while (!feof(src));

	rewind(src);
	return ret;
}

char * getline(FILE *src) {
	char *ret = malloc(81);
	fgets(ret, 80, src);
	replace(ret, ';', '\n');
	__instruction_location++;

	return ret;
}

Statement * Parse(char *line) {
	Statement *ret = NewStatement();
	char *token;
	char *strlit = NULL; /* */
	char *extra; /* for string literals */
	char *tab;
	char *comma = ",";
	char *nl = "\n";
	bool onearg = false;
	char *commands[] = {
		"INT", "FLT", "STR", "ADD", "SUB", "MUL", "DIV", "MOD",
		"INC", "DEC", "MOV", "CAT", "RIN", "OUT", "FIL", "OPN",
		"CLS"
	};
	int temp;

	while ((tab = strchr(line, '\t')) != NULL) {
		*tab = ' ';
	}
	
	token = strtok(line, " ");
	if (token[0] == '\n' || token[0] == ';') {
		return NULL;
	}
	ToUpper(token);

	/* Check the COMMANDS enum in types.h */
	ret->command = arraystr(commands, __instruction_count, token);
	if (ret->command == -1) {
		ABORT("Error: fuck unknown keyword: %s", token);
	}

	token = strtok(NULL, " ");
	if (!token) {                     // 0 arguments
		ret->argcount = 0;
		return ret;
	}

	comma = strchr(token, ',');             // check if token exists as var
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
		return ret;
	}
	if (token[0] == ';') {
		ret->argcount = 1;
		ret->args[1] = NULL;
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
		DEBUGMSG("[PARSE] %d: String literal with spaces\n", __LINE__);
		strlit = malloc(80);
		strlit[0] = '\0';
		strncat(strlit, token, 31);
		while ((extra = strtok(NULL, " ")) != NULL) {
			replace(extra, '\n', '\0');
			DEBUGMSG("[PARSE] Appending %s to string\n", extra);
//			printf("extra: %s\n", extra);
			strcat(strlit, " ");
			strcat(strlit, extra);
		}
	}
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
	DEBUGMSG("[PARSE] %d: token: \"%s\"\n", __LINE__, token);

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
	DEBUGMSG("[PARSE] Creating variable argument for \"%s\"\n", token);
	Arg *ret;
	int len = strlen(token);

	for (int i = 0; i < len; i++) {
		if (!isalnum(token[i]) && token[i] != '_') {
			ABORT("Illegal character in variable name: %s", token);
		}
	}
	
	ret = malloc(sizeof(Arg));

	DEBUGMSG("[PARSE] Searching environment for \"%s\"\n", token);
	if ((ret->var = Env(token)) != NULL) {
		DEBUGMSG("[PARSE] Found %s in environment\n", ret->var->label);
		return ret;
	}
	DEBUGMSG("[PARSE] \"%s\" not found in environment\n", token);

	ret->isliteral = false;
	ret->token = token;
	return ret;
}

Var * Env(char *token) {
	Var *ret = NULL;

	DEBUGMSG("[PARSE] Environment contains %d variables\n", env->varcount);
	for (int i = 0; i < env->varcount; i++) {
		DEBUGMSG("[PARSE] \t%s variable: \"%s\"\n", TypeLabel(env->vars[i]->type),
		       env->vars[i]->label);
		if (strcmp(env->vars[i]->label, token) == 0) {
			DEBUGMSG("[PARSE] Found %s in environment at %d\n",
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
/*		if (st->args[i]->var->type == _STR && st->args[i]->isliteral) {
			free(ARGVAL(st->args[i], STR));
		}
*/		free(st->args[i]);
	}

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
