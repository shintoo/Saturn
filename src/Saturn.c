#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Saturn.h"

int __linecount = 0;

Environment env;

const char *__COMMANDS2  =
	"MOV ADD SUB MUL DIV "
	"OUT RIN ";
const char *__COMMANDS1  =
	"INC DEC ";
const char *__COMMANDS12 =
	"INT STR FLT ";

void Init(void) {
	env.vars = malloc(sizeof(20 * sizeof(Var *)));
	env.memsize = 20;
	env.varcount = 0;
}

void End(void) {
	for (int i = 0; i < env.varcount; i++) {
		free(env.vars[i]);
	}
	free(env.vars);
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

Statement * Parse(char *line) {
	__linecount++;

	Statement *ret = NewStatement();
	char *token;
	char *tab;
	char *comma = ",";
	char *nl = "\n";

	while ((tab = strchr(line, '\t')) != NULL) {
		*tab = ' ';
	}
	
	token = strtok(line, " ");
	if (!token) { printf("empty line"); return NULL; }
	ToUpper(token);
	ret->command = token;

	token = strtok(NULL, " ");
	if (!token) {                     // 0 arguments
		ret->argcount = 0;
		return ret;
	}

	comma = strchr(token, ',');             // check if token exists as var
	if (comma == NULL) {
		Abort("Missing comma after argument: ", token);
	}
	*comma = '\0';

	ret->args[0] = CreateArg(token);

	token = strtok(NULL, " ");
	if (!token) {                    // 1 argument
		ret->argcount = 1;
		return ret;
	}
	
	nl = strchr(token, '\n');
	if (nl) {
		*nl = '\0';
	}

	ret->argcount = 2;              // 2 arguments
	ret->args[1] = CreateArg(token);

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
		  Abort("Illegal character beginning argument: ", token);
	}

	newarg = CreateVarArg(token); // Adds to env

	return newarg;
}

Arg * CreateStringLiteral(char *token) {
	char *end;
	Arg *ret;

	if (token[strlen(token) - 1] != '\'') {
		if (strchr(token + 1, '\'') == NULL) {
			Abort("Missing terminating \' for: ", token);
		}
		Abort("\' must end string literal: ", token);
	}
	for (int i = 0; token[i] != '\0'; i++) {
		token[i] = token[i + 1];
	}

	end = strchr(token, '\'');
	*end = '\0';
	
	ret = malloc(sizeof(Arg));
	ret->isliteral = true;
	ret->type = _STR;
	ret->val.STR = token;

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
			Abort("Numeric literals may only contain digits and a decimal: ", token);
		}
	}

	ret = malloc(sizeof(Arg));
	ret->isliteral = true;
	if (flt) {
		ret->type = _FLT;
		ret->val.FLT = atof(token);
	} else {
		ret->type = _INT;
		ret->val.INT = atoi(token);
	}

	return ret;
}

Arg * CreateVarArg(char *token) {
	Arg *ret;
	int len = strlen(token);

	for (int i = 0; i < len; i++) {
		if (!isalnum(token[i]) && token[i] != '_') {
			Abort("Illegal character in variable name: ", token);
		}
	}
	
	ret = malloc(sizeof(Arg));
	
	if ((ret->var = Env(token)) != NULL) {
		return ret;
	}
	
	ret->var = malloc(sizeof(Var));
	ret->var->label = token;
	
	if (env.varcount == env.memsize) {
		env.vars = realloc(env.vars, 10);
		env.memsize += 10;
	}
	env.vars[env.varcount] = ret->var;
	env.varcount++;

	return ret;
}

Var * Env(char *token) {
	for (int i = 0; i < env.varcount; i++) {
		if (strcmp(env.vars[i]->label, token)) {
			return env.vars[i];
		}
	}
	return NULL;
}

void Validate(const Statement * st) {
	if (st->argcount == 0) {
		Abort("Too few arguments supplied for: ", st->command);
	}
	if (!strstr(__COMMANDS2, st->command) && !strstr(__COMMANDS1, st->command)
		&& !strstr(__COMMANDS12, st->command)) {
		Abort("Unrecognized command: ", st->command);
	}
	if (strstr(__COMMANDS2, st->command) && st->argcount != 2) {
		Abort("Expected two arguments for: ", st->command);
	}
	if (strstr(__COMMANDS1, st->command) && st->argcount != 1) {
		Abort("Expected one argument for: ", st->command);
	}

}

Statement * NewStatement(void) {
	Statement *newst = malloc(sizeof(Statement));
	newst->args = malloc(2 * sizeof(Arg *));

	return newst;
}

void DeleteStatement(Statement *st) {
	for (int i = 0; i < st->argcount; i++) {
		free(st->args[i]);
	}
	free(st);
}

void Abort(char *error, char *detail) {
	printf("%d: %s%s\n", __linecount, error, detail);
	exit(EXIT_FAILURE);
}

const char * TypeLabel(enum types type) {
	const char *TYPELABELS[] = {
		"INT", "FLT", "STR", "CHR", "FIL"
	};
	return TYPELABELS[type];
}

void ToUpper(char *st) {
	int len = strlen(st);

	for (int i = 0; i < len; i++) {
		st[i] = toupper(st[i]);
	}
}
