#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

extern int __linecount;
FILE *src_file;
char StatusWord;

int main(int argc, char **argv) {
	char line[81];
	int linecount;
	bool interactive = false;
	char *comment = NULL;
	int envflags; // rename later

	StatusWord = 0;
	if (argc != 1) {
		src_file = fopen(argv[1], "r");
		linecount = CountLines(src_file);
	}
	else {
		src_file = stdin;
		linecount = -1;
		interactive = true;
	}

	if (interactive) {
		printf("Saturn v0.1.2\nEnter \'help\' for more information.\n"
			"Enter 'quit' to exit.\n");
	}
	Init();
	Statement *instruction;
	for (int i = 0; !feof(src_file); i++) {
		if (interactive) {
			printf("saturn> ");
		}
		fgets(line, 80, src_file);
		if (feof(src_file)) {
			break;
		}		
#ifdef DEBUG
		getchar();
#endif
		__linecount++;

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

		/* Skip empty lines and labels*/
		if (instruction == NULL) {
			DEBUGMSG("<blank line or label>\n");
			free(instruction);
			continue;
		}

		/* Validate the statement */
		Validate(instruction);

		/* Execute the instruction */
		Execute(instruction);

		/* Delete the statement, free all memory used for it and for literals */
		DeleteStatement(instruction);

	}

	if (argc != 1) {
		fclose(src_file);
	}
	End();
	return 0;
}
