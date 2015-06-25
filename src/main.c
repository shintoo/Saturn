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
		getchar();

		/* Parse line into a statement */
		instruction = Parse(line);

		/* Skip empty lines */
		if (instruction == NULL) {
			printf("<blank line>\n");
			continue;
		}

		/* Validate the statement */
//		Validate(instruction);
		
		/* Press enter to step through, one line at a time */
		getchar();

		putchar('\n');
		/* Execute the instruction */
		Execute(instruction);
		getchar();

		putchar('\n');

		/* Print the statement after execution */
		PrintStatement(instruction);

		/* Delete the statement, free all memory used for it and for literals */
		DeleteStatement(instruction);

	}

	fclose(src);
//	End();
	return 0;
}

void PrintStatement(const Statement *st) {
	printf("\tCommand: %d\n\t\tArguments:\n", st->command);
	for (int i = 0; i < st->argcount; i++) {
		printf("\t\t\t%s", TypeLabel(st->args[i]->var->type));
		if (st->args[i]->isliteral) {
			printf(" Literal\n");
		} else {
			printf(" Variable\n");
		}
		printf("\t\t\tLabel: %s\n\t\t\tValue: ", st->args[i]->var->label);
		switch (st->args[i]->var->type) {
			case _INT: printf("%d\n", st->args[i]->var->val.INT); break;
			case _FLT: printf("%f\n", st->args[i]->var->val.FLT); break;
			case _STR: printf("%s\n", st->args[i]->var->val.STR); break;
		}
		putchar('\n');
	}
}

