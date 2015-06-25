#ifndef _TYPES_H_
#define _TYPES_H_
#include <stdio.h>
#include <stdbool.h>

enum types {_INT, _FLT, _STR, _FIL };
enum tokentypes { _ARG, _STATEMENT };

enum COMMANDS {
	INT, FLT, STR,
	ADD, SUB, MUL, DIV,
	INC, DEC,
	MOV, RIN, OUT
};

typedef union _val {
	int INT;
	float FLT;
	char CHR;
	char *STR;
	FILE *FIL;
} Val;

/* the variable */
typedef struct _var {
	char *label;
	enum types type;
	Val val;
} Var;


struct _arg;

/* A single line consisting of an
 * command and it's arguments(s);
 * ex: "INT M" or "ADD M, 1"
 */
typedef struct _statement {
	enum COMMANDS command;
	int argcount;
	struct _arg **args;
} Statement;

/* The argument to a statement */
typedef struct _arg {
	char *token;    // Used for creating variables
	bool isliteral;
	Var *var;
} Arg;

typedef struct _env {
	int varcount;
	int memsize;
	Var **vars;
} Environment;

#endif
