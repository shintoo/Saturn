#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_
#include "types.h"

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

#endif
