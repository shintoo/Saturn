#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"

extern int __linecount;

void PrintStatement(const Statement *st);

int main(int argc, char **argv) {
	FILE *src;
	char line[32];
	int linecount;

	if (argc != 1) {
		src = fopen(argv[1], "r");
		linecount = CountLines(src);
	}
	else {
		puts("saturn: fatal error: no input files\ninterpretation terminated.");
		exit(EXIT_FAILURE);
	}

	Init();

	Statement *instruction;
	
	for (int i = 0; i < linecount; i++) {
		fgets(line, 32, src);
		printf("%d: %s", i, line);

		/* Parse line into a statement */
		instruction = Parse(line);

		/* Skip empty lines */
		if (instruction == NULL) {
			printf("<blank line>\n");
			continue;
		}
		
		/* Validate a statement - check args, command, etc */
		Validate(instruction);

		/* This function is not part of the interpreter, just used
		 * here for demonstration. */
		PrintStatement(instruction);
		
		/* Execute the instruction */
		Execute(instruction);

		/* Delete the statement, free all memory used for it and for literals */
		DeleteStatement(instruction);

	}
	
	End();
	return 0;
}

void PrintStatement(const Statement *st) {
	printf("\tCommand: %s\n\t\tArguments:\n", st->command);
	for (int i = 0; i < st->argcount; i++) {
		if (st->args[i]->isliteral) {
			printf("\t\t\t%s Literal\n\t\t\tValue: ",
			        TypeLabel(st->args[i]->type));
			switch(st->args[i]->type) {
				case _INT: printf("%d\n", st->args[i]->val.INT); break;
				case _FLT: printf("%f\n", st->args[i]->val.FLT); break;
				case _STR: printf("%s\n", st->args[i]->val.STR); break;
			}
		} else {
			printf("\t\t\t%s Variable\n\t\t\tLabel: %s\n\t\t\tValue: ",
			       TypeLabel(st->args[i]->var->type), st->args[i]->var->label);
			switch (st->args[i]->var->type) {
				case _INT: printf("%d\n", st->args[i]->var->val.INT); break;
				case _FLT: printf("%f\n", st->args[i]->var->val.FLT); break;
				case _STR: printf("%s\n", st->args[i]->var->val.STR); break;
			}
		}
		putchar('\n');
	}
}

