#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Saturn.h"

int __linecount = 0;

const char *__COMMANDS2  =
	"MOV ADD SUB MUL DIV "
	"OUT RIN ";
const char *__COMMANDS1  =
	"INC DEC ";
const char *__COMMANDS12 =
	"NEW ";

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

char *GetLine(FILE *src) {
	char line[32];
	char *ret = malloc(32);
	char *token;
	__linecount++;

	fgets(line, 32, src);
	char *tab;
	while ((tab = strchr(line, '\t')) != NULL) {
		*tab = ' ';
	}
	token = strtok(line, " ");
	while (token != NULL) {
		strcat(ret, token);
		strcat(ret, " ");
		token = strtok(NULL, " ");
	}

	return ret;
}

Statement * ProcessLine(const char *line) {
	Statement *ret = NewStatement();

	if ((ret->command = GetCommand(line)) == NULL) {
		return NULL;
	}
	ret->argcount = CountArgs(line);

	if ((ret->args[0] = GetArg(line, 1)) == NULL) {
		Abort("No arguments provided for ", ret->command);
	}

	if (strstr(__COMMANDS1, ret->command)) {
		if (ret->argcount != 1) {
			Abort(ret->command, " expects one argument");
		}
	} else if (strstr(__COMMANDS2, ret->command)) {
		if (ret->argcount != 2) {
			Abort(ret->command, " expects two arguments");
		}
		ret->args[1] = GetArg(line, 2);
	} else if (strstr(__COMMANDS12, ret->command)) {
		if  (ret->argcount != 1 && ret->argcount != 2) {
			Abort(ret->command, " expects one or two arguments");
		}
		if (ret->argcount == 2) {
			ret->args[1] = GetArg(line, 2);
		}
	}

	return ret;
}

int CountArgs(const char *line) {
	char ch = line[0];
	int i;
	
	for (i = 0; isspace(ch); i++) ch = line[i];
	for (; !isspace(ch); i++) ch = line[i];
	for (; isspace(ch); i++) {
		ch = line[i];
		if (ch == '\n') return 0;
	}
	for (; !isspace(ch); i++) ch = line[i];
	for (; isspace(ch); i++) {
		ch = line[i];
		if (ch == '\n') return 1;
	}
	return 2;
}

Statement * NewStatement(void) {
	Statement *newst = malloc(sizeof(Statement));
	newst->args = malloc(2 * sizeof(Arg *));

	return newst;
}

void DeleteStatement(Statement *st) {
	free(st->command);
	for (int i = 0; i < st->argcount; i++) {
		if (strstr(st->args[i]->var->label, "_LITERAL")) {
			free(st->args[i]->var);
			free(st->args[i]);
		}
	}
	free(st);
}

char * GetCommand(const char *line) {
	char prev = line[0];
	char curr;
	char *command = malloc(8);
	int i = 1;
	int j = 1;

	while (isspace(prev)) {
		if (prev == '\n')
			return NULL;
		prev = line[i++];
	}

	command[0] = prev;
	curr = line[i];
	
	while (!isspace(curr)) {
		prev = curr;
		curr = line[i++];
		command[j++] = curr;
		if (j > 4) Abort("Error: Invalid instruction", "");
	}

	command[j] = '\0';
	
	for (int i = 0; i < j; i++)
	    command[i] = toupper(command[i]);
/*
	if (strstr(__COMMANDS, command) == NULL)
	    Abort("Error: Invalid instruction: ", command);
*/
	return command;
}

Arg * GetArg(const char *line, int index) {
	Arg *newarg = malloc(sizeof(Arg));
	newarg->statement = false;
	char tok[32];
	int i = 0;
	char ch = line[i];
	int j;
	
	/* Empty line, or reach first token (command) */
	for (; isspace(ch); i++) {
		ch = line[i];
		if (ch == '\n') return NULL;
	}
	/* line[i] is now the first char of the command */

	/* Skip the command */
	for (; !isspace(ch); i++) {
		ch = line[i];
	}
	--i;
	/* line[i] is now the first whitespace after the command */
	
	/* Check for arguments */
	for (; isspace(ch); i++) {
		ch = line[i];
		if (ch == '\n') {
			return NULL;
		}
	}
	--i;
	/* line[i] is now the first char of the first arg */
	
	/* Skip args until reaching the desired arg */
	if (index == 2) {
		for (; ch != ',' && ch != '\n'; i++) ch = line[i];
		if (ch == '\n')
			Abort("Missing comma after ", GetArg(line, 1)->var->label);
		/* line[i] is now the first whitespace after the arg */

		for (; isspace(ch); i++) {
			if (ch == '\n') return NULL;
			ch = line[i];
		}
		i++;
		/* line[i] is now the first char of the next arg */
	}
	/* line[i] is now the first char of the desired arg */

	for (j = 0; !isspace(line[i]) && line[i] != ','; j++, ++i) {
		tok[j] = line[i];
	}
	tok[j] = '\0';
	/* tok is now the desired arg */

	/*********** Begin processing token ***********/
	if ((strchr("\"\'abcdefghijklmnopqrstuvwxyz_", tolower(tok[0])) == NULL)
		&& !isdigit(tok[0])) {
		Abort ("Invalid character begins variable name: ", "");
	}

	/* numeric literals */
	if (isdigit(*tok)) {
		/* Check validity of numeric literal */
		for (i = 0; i < j; i++) {
			if (!isdigit(tok[i]) && tok[i] != '.') {
				Abort("Invalid token", "");
			}
		}
		/* Create literal */
		newarg->var = malloc(sizeof(Var));
		/* Contains a '.' - float literal */
		if (strchr(tok, '.')) {
			newarg->var->label = "__FLT_LITERAL";
			newarg->var->type = _FLT;
			newarg->var->val.FLT = atof(tok);
			return newarg; /* RETURN FLOAT LITERAL */
		/* Contains no '.' - integer literal */
		} else {
			newarg->var->label = "__INT_LITERAL";
			newarg->var->type = _INT;
			newarg->var->val.INT = atoi(tok);
			return newarg; /* RETURN INTEGER LITERAL */
		}
	}

	/* character literals */
	else if (tok[0] == '\'') {
		if (tok[1] == '\0') Abort("Error: Missing closing \'", ""); 
		if (tok[2] == '\'') {
			newarg->var = malloc(sizeof(Var));
			newarg->var->label = "__CHR_LITERAL";
			newarg->var->type = _CHR;
			newarg->var->val.CHR = tok[1];
			return newarg; /* RETURN CHARACTER LITERAL */
		} else Abort("Error: Character literals must be one character: ", tok);
	}
	
	return NULL;
/*
	else {
		Arg *temp;
		if((temp = VarExists(tok)) 
	}
*/
}

void Abort(char *error, char *detail) {
	printf("%d: %s: %s\n", __linecount, error, detail);
	exit(EXIT_FAILURE);
}

const char * TypeLabel(enum types type) {
	const char *TYPELABELS[] = {
		"INT", "FLT", "STR", "CHR", "FIL"
	};
	return TYPELABELS[type];
}
