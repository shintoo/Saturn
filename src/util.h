#ifndef _UTIL_H_
#define _UTIL_H_

//#define DEBUG
#define VERSION 0.1

#define OUT_STDOUT_NEWLINE(X) \
	(X)->command == OUT ? (X)->args[1]->var->type == _STR ? (X)->args[1]->var->val.STR[0] == '\n' ? 1 : 0 : 0 : 1

#define INT_OR_FLT(X) \
	((X)->var->type == _INT ? (X)->var->val.INT : (X)->var->val.FLT)

#define ARITHMETIC(DST, OP, SRC) \
	switch ((DST)->var->type) { \
		case _INT: (DST)->var->val.INT OP INT_OR_FLT(SRC); break; \
		case _FLT: (DST)->var->val.FLT OP INT_OR_FLT(SRC); break; \
	}

typedef enum {
	help_int, help_flt, help_str, help_mov, help_add, help_mul,
	help_sub, help_div, help_rin, help_out, help_inc, help_dec
} helptag;

void Help(void);

void DisplayTopic(int topic);

int arraystr(char **arr, int nmemb, char *str);

#endif
