#include <stdio.h>
#include <stdlib.h>
#include "Saturn.h"

extern int __linecount;

void PrintStatement(const Statement *st);

int main(int argc, char **argv) {
	FILE *src = fopen(argv[1], "r");
	int linecount = CountLines(src);
	char *line;

	Statement *instruction;
	for (int i = 0; i < linecount; i++) {
		/* Parse the line */
		line = GetLine(src);
		printf("Line %d: %s", __linecount, line);
		instruction = ProcessLine(line);
		if (instruction  == NULL) {
			continue;
		}
	
		/* This function is not part of the interpreter, just used
		 * here for demonstration. */
		PrintStatement(instruction);

		DeleteStatement(instruction);
	}

	return 0;
}

void PrintStatement(const Statement *st) {
	printf("\tCommand: %s\n\t\tArguments: %d\n", st->command, st->argcount);
	for (int i = 0; i < st->argcount; i++) {
		printf("\t\t\tLabel: %s\n\t\t\tType: %s\n\t\t\tValue: ",
		        st->args[i]->var->label, TypeLabel(st->args[i]->var->type));
		switch (st->args[i]->var->type) {
			case _INT: printf("%d\n", st->args[i]->var->val.INT); break;
			case _FLT: printf("%g\n", st->args[i]->var->val.FLT); break;
			case _CHR: printf("\'%c\'\n", st->args[i]->var->val.CHR); break;
			case _STR: printf("\"%s\"\n", st->args[i]->var->val.STR); break;
		}
		putchar('\n');
	}

}
