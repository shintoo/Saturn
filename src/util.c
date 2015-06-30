#include <stdio.h>
#include <string.h>
#include "util.h"

int arraystr(char **arr, int nmemb, char *str) {
	for (int i = 0; i < nmemb; i++) {
		if (strcmp(arr[i], str) == 0) {
			return i;
		}
	}
	return -1;
}

void DisplayTopic(int topic) {
	char *dialogues[] = {
		"this is the dialogue for int", "this is the dialogue for flt",
		"this is the dialogue for str", "this is the dialogue for mov",
		"this is the dialogue for add", "this is the dialogue for mul",
		"this is the dialogue for sub", "this is the dialogue for div",
		"this is the dialogue for rin", "this is the dialogue for out",
		"this is the dialogue for dec", "this is the dialogue for dec"
	};

	puts(dialogues[topic]);
}
void Help(void) {
	printf("\nWelcome to Saturn version %.1f!\n\nEnter a keyword, type, or"
		" topic for more information.\nEnter 'quit' to return to Saturn.\n", VERSION);
	char input[32] = { ' '};
	char *topics[] = {
		"int", "flt", "str", "mov", "add", "mul", "sub",
		"div", "rin", "out", "inc", "dec",
	};
	int topic;

	for (;;) {
		printf("help> ");
		fgets(input, 32, stdin);
		*strchr(input, '\n') = '\0';
		if (strncmp(input, "quit", 4) == 0) {
			break;
		}
		if (strncmp(input, "list", 4) == 0) {
			for (int i = 0; i < 12; i++) {
				if (i > 0 && i % 3 == 0) {
					putchar('\n');
				}
				printf("%s\t", topics[i]);
			}
			putchar('\n');
			continue;
		}
		topic = arraystr(topics, 12, input);
		if (topic == -1) {
			puts("Invalid option, for a list of options, enter 'list'");
			continue;
		}
		DisplayTopic(topic);
		}
}
