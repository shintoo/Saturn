#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"

//#define DEBUG

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
		if (line[0] == '#') {
			continue;
		}
#ifdef DEBUG
		printf("%d: %s", i, line);
		getchar();
#endif
		/* Parse line into a statement */
		instruction = Parse(line);

		/* Skip empty lines */
		if (instruction == NULL) {
#ifdef DEBUG
			printf("<blank line>\n");
#endif
			continue;
		}

		/* Validate the statement */
		Validate(instruction);
#ifdef DEBUG
		/* Press enter to step through, one line at a time */
		getchar();

		putchar('\n');
#endif
		/* Execute the instruction */
		Execute(instruction);
#ifdef DEBUG
		getchar();

		putchar('\n');

		/* Print the statement after execution */
//		PrintStatement(instruction);
#endif
		/* Delete the statement, free all memory used for it and for literals */
		DeleteStatement(instruction);

	}

	fclose(src);
	End();
	return 0;
}

