#ifndef _MYLANG_H_
#define _MYLANG_H_
#include <stdio.h>
#include <stdbool.h>

enum types {_INT, _FLT, _STR };
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
	bool isliteral;
	union {
		Var *var;            // Variable
		struct {             // Literal
			enum types type;
			Val val;
		};
	};
} Arg;

typedef struct _env {
	int varcount;
	int memsize;
	Var **vars;
} Environment;

/*********************************************************/
/*                Environment Functions                  */
/*********************************************************/

/* Initialize the memory for the environment */
void Init(void);

/* Find if a variable is in the environment from
 * it's label */
 Var *Env(char *token);

/* Delete the environment */
void End(void);



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

/* Parse a single line into an executable statement */
Statement * Parse(char *line);

/* Create an argument for a statement from a token */
Arg * CreateArg(char *token);

/* Create a string literal argument from a token */
Arg * CreateStringLiteral(char *token);

/* Create a numeric (int or float) literal argument 
 * for a statement from a token */
Arg * CreateNumericLiteral(char *token);

/* Create a variable argument from a token, either
 * fetching a pre-existing variable from the
 * environment or create a new variable */
Arg * CreateVarArg(char *token);

/* Validate a statement, aborting interpretation
 * upon detecting an invalid statement */
void Validate(const Statement *st);

/* Create a new statement */
Statement * NewStatement(void);

/* Delete a statement, but not it's Args */
void DeleteStatement(Statement *st);

/* Execute a statement */
void Execute(const Statement *st);

/*********************************************************/
/*              Instruction Functions                    */
/*********************************************************/

void smov(Arg *dst, const Arg *src);

void sadd(Arg *dst, const Arg *src);

void smul(Arg *dst, const Arg *src);

void sdiv(Arg *dst, const Arg *src);

void sinc(Arg *dst);

void sdec(Arg *dst);

void srin(Arg *dst, const Arg *src);

void sout(Arg *dst, const Arg *src);

/* new instruction */
void snew(Arg *dst, const Arg *src);

/*********************************************************/
/*                   Misc Functions                      */
/*********************************************************/

/* Count the amount of lines in a file */
int CountLines(FILE *src);

/* Retreive the line from the file                       */
char * GetLine(FILE *source);

/* Abort interpretation and print error, 
 * including the line number and a detail.
 * e.g.: "15: Too few arguments for MOV"
 */
void Abort(char *error, char *detail);

const char * TypeLabel(enum types type);

void ToUpper(char *st);

#endif
