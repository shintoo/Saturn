#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

int main(int argc, char **argv) {
	FILE *src;
	char line[81];
	int linecount;
	bool interactive = false;
	char *comment = NULL;

	if (argc != 1) {
		src = fopen(argv[1], "r");
		linecount = CountLines(src);
	}
	else {
		src = stdin;
		linecount = -1;
		interactive = true;
	}

	if (interactive) {
		printf("Saturn 0.1\nEnter \'help\' for more information.\n"
			"Enter 'quit' to exit.\n");
	}
	Init();
	Statement *instruction;
	for (int i = 0; i != linecount; i++) {
		if (interactive) {
			printf("saturn> ");
		}
		fgets(line, 80, src);
		
		comment = strchr(line, ';');
		if (comment) {
			*comment = '\n';
		}
		
		if (interactive) {
			if (strncmp(line, "help", 4) == 0) {
				Help();
				continue;
			}
			if (strncmp(line, "quit", 4) == 0) {
				break;
			}
		}
		if (line[0] == '#') {
			continue;
		}
		DEBUGMSG("%d: %s", i, line);
		/* Parse line into a statement */
		instruction = Parse(line);

		/* Skip empty lines */
		if (instruction == NULL) {G
			DEBUGMSG("<blank line>\n");
			continue;
		}

		/* Validate the statement */
		Validate(instruction);

		/* Execute the instruction */
		Execute(instruction);

		/* Append a newline to all commands unless printing a newline */
		if (interactive) {
			if (out_stdout(instruction)) {
				putchar('\n');
			}
		}

		/* Delete the statement, free all memory used for it and for literals */
		DeleteStatement(instruction);

	}

	if (argc != 1) {
		fclose(src);
	}
	End();
	return 0;
}
