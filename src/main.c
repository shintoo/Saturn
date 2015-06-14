#include <stdio.h>
#include <stdlib.h>
#include "Saturn.h"

void PrintStatement(const Statement *st);

int main(void) {
	/* These lines do not make sense; their purpose is to merely demonstrate */
	char *lines[] = {
		"add 'G', 76\n",
		"sub 6.5, 10\n",
		"\n",
		"mul 10, 5.3\n",
		"inc 15\n"
	};
	/* All lines are terminated with a newline, any other 
	 * whitespace is insignificant,
	 * however, there must be whitespace between tokens */

	Statement *instruction;
	for (int i = 0; i < 4; i++) {
		/* Parse the line */
		printf("Line %d: %s", i, lines[i]);
		if ((instruction = ProcessLine(lines[i])) == NULL) {
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
		
	}

}
