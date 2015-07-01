#ifndef _UTIL_H_
#define _UTIL_H_

//#define DEBUG
#define VERSION 0.1

#define ARGVAL(ARG, TYPE) \
	(ARG)->var->val.TYPE

#define INT_OR_FLT(X) \
	((X)->var->type == _INT ? (X)->var->val.INT : (X)->var->val.FLT)

#define ARITHMETIC(DST, OP, SRC) \
	switch ((DST)->var->type) { \
		case _INT: (DST)->var->val.INT OP INT_OR_FLT(SRC); break; \
		case _FLT: (DST)->var->val.FLT OP INT_OR_FLT(SRC); break; \
	}

#define MAKE_ARITHMETIC_FUNCTION(NAME, OP) \
void saturn_##NAME(Arg *dst, const Arg *src) { \
	if ((dst->var->type | src->var->type) > 1) { \
		ABORT("May only %s numeric values\n", #NAME); \
	} \
	if (dst->var->isconst) { \
		ABORT("Error: constant variable: %s", dst->var->label); \
	} \
	ARITHMETIC(dst, OP, src); \
}

#define ABORT(...) \
	printf("%d:", __linecount); \
	printf(__VA_ARGS__); \
	putchar('\n'); \
	End(); \
	exit(EXIT_FAILURE);

#ifdef DEBUG
#define DEBUGMSG(...) \
	fprintf(stderr, ...);
#else
#define DEBUGMSG(...)
#endif

bool out_stdout(Statement *st);

void Help(void);

void DisplayTopic(int topic);

int arraystr(char **arr, int nmemb, char *str);

#endif

