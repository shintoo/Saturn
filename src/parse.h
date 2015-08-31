#ifndef _PARSE_H_
#define _PARSE_H_
#include <stdio.h>
#include <stdbool.h>
#include "types.h"

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

/* Scrape for labels */
Label * ScrapeForLabels(FILE *src);

/* find a label, returns 1 if found, 0 if not */
int FindLabel(const char *label, fpos_t *loc);

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
