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

bool replace(char *str, char old, char new) {
	int len = strlen(str);
	bool found = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == old) {
			found = true;
			str[i] = new;
		}
	}

	return found;
}
