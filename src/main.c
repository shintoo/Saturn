#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

#define VERSION 0.1

void Help(void);

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
		printf("Saturn 0.1\nEnter \"help\" for more information.\n");
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
#ifdef DEBUG
		printf("%d: %s", i, line);
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

		/* Execute the instruction */
		Execute(instruction);

		/* Append a newline to all commands unless printing a newline */
		if (interactive) {
			if (!(OUT_STDOUT_NEWLINE(instruction))) {
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

void Help(void) {
	printf("\nWelcome to Saturn version %.1f!\n\nEnter a keyword, type, or"
		" topic for more information.\n", VERSION);
	char input[32] = { ' '};

	while (strncmp(input, "quit", 4) != 0) {
		printf("help> ");
		fgets(input, 32, stdin);
	}

}
