#ifndef _TYPES_H_
#define _TYPES_H_
#include <stdio.h>
#include <stdbool.h>

enum types {_INT, _FLT, _STR, _FIL, _LABEL };

enum COMMANDS {
	INT, FLT, STR,
	ADD, SUB, MUL, DIV, MOD,
	INC, DEC,
	MOV, CAT, RIN, OUT
};

typedef struct _label {
	char *string;
	fpos_t loc;
} Label;

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
	bool isconst;
	enum types type;
	Val val;
} Var;

/* The argument to a statement */
typedef struct _arg {
	char *token;    // Used for creating variables
	bool isliteral;
	bool islabel;
	union {
		Var *var;
		Label label;
	};
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
