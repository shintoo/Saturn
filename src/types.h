#ifndef _TYPES_H_
#define _TYPES_H_
#include <stdio.h>
#include <stdbool.h>

enum types {_INT, _FLT, _STR, _FIL};

enum COMMANDS {
	INT, FLT, STR, FIL,
	ADD, SUB, MUL, DIV, MOD,
	INC, DEC,
	MOV, CAT, GET, OUT, OPN, CLS,
	CMP, JMP, JEQ, JNE, JIG, JIL, JGE, JLE
};

struct _file {
	FILE *pntr;
	char *path;
	char *mode; /* C style file modes, (rwab+) */
	bool isopen;
};

typedef union _val {
	int INT;
	float FLT;
	char *STR;
	struct _file FIL;
} Val;

/* the variable */
typedef struct _var {
	char *label;
	bool isconst;
	enum types type;
	Val val;
} Var;

/* The argument to a statement */
typedef struct _arg {
	char *token;
	bool isliteral;
	Var *var;
} Arg;

/* A single line consisting of an
 * command and it's arguments(s);
 * ex: "INT M" or "ADD M, 1"
 */
typedef struct _statement {
	enum COMMANDS command;
	int argcount;
	struct _arg **args;
} Statement;

/* Holds the list of variables declared with
 * int, flt, or str */
typedef struct _env {
	int varcount;
	int memsize;
	Var **vars;
} Environment;

#endif
