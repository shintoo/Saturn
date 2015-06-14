#ifndef _MYLANG_H_
#define _MYLANG_H_
#include <stdio.h>
#include <stdbool.h>

enum types {_INT, _FLT, _STR, _CHR, _FIL };
enum tokentypes { _ARG, _STATEMENT };

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

/* Allow for _arg union to be a member
 * of Statement structure
 */
struct _arg;

/* A single line consisting of an
 * command and it's arguments(s);
 * ex: "INT M" or "ADD M, 1"
 */
typedef struct _statement {
	int linenum;
	char *command;
	int argcount;
	struct _arg **args;
} Statement;

/* Either a variable or a statement.
 * e.g.: in "ADD N, M", argument 1
 * is a Var. in "CND EQL N, M",
 * argument 1 is a statement, and
 * the only argument.
 */
typedef struct _arg {
	bool statement;
	union {
		Var *var;
		Statement *st;
	};
} Arg;

/*********************************************************/
/*                   Var Functions                       */
/*********************************************************/

/* Creates an INT variable with the label and value */
Var * NewINT(const char *label, const int val);

/* Creates a FLT variable with the label and value */
Var *NewFLT(const char *label, const float val);

/* Creates a STR variable with the label and value */
Var *NewSTR(const char *label, const char *val);

/* Creates a CHR variable with the label and value */
Var *NewCHR(const char *label, const char val);

/* Creates a FIL variable with the label, the name
 * of the file, and the type to open it with       */
Var *NewFIL(const char *label, const char *name, const char *opts);

/* Deletes a Var and it's contents */
void DelVar(Var *v);

/*********************************************************/
/*                      Arg Functions                    */
/*********************************************************/

/* Get an argument from a line at an index,
 * First argument is 1, second is 2.
 * Returns NULL if no argument at index.
 */
Arg * GetArg(const char *line, int index);

/* Count the number of arguments in a line,
 * up to two arguments.
 */
int CountArgs(const char *line);


/*********************************************************/
/*                 Statement Functions                   */
/*********************************************************/

/* Create a new statement */
Statement * NewStatement(void);

/* Delete a statement, but not it's Args */
void DeleteStatement(Statement *st);

/* Processes a single line into a Statement
 * with or without argument(s). Aborts on error.
 */
Statement * ProcessLine(const char *line);

/* Used in ProcessLine() to retrieve the command.
 * Aborts if invalid, returns NULL on blank lines.
 */
char * GetCommand(const char *line);

/* Execute a statement */
void Execute(const Statement *st);

/*********************************************************/
/*              Instruction Functions                    */
/*********************************************************/

void MOV(Arg *dst, const Arg *src);

void ADD(Arg *dst, const Arg *src);

void MUL(Arg *dst, const Arg *src);

void DIV(Arg *dst, const Arg *src);

void INC(Arg *dst);

void DEC(Arg *dst);

void INT(Arg *dst, const Arg *src);

void FLT(Arg *dst, const Arg *src);

void CHR(Arg *dst, const Arg *src);

void STR(Arg *dst, const Arg *src);

/*********************************************************/
/*                   Misc Functions                      */
/*********************************************************/

/* Retreive the line from the file                       */
char * GetLine(FILE *source, int line);

/* Abort interpretation and print error, 
 * including the line number and a detail.
 * e.g.: "15: Too few arguments for MOV"
 */
void Abort(char *error, char *detail);

const char * TypeLabel(enum types type);

#endif
