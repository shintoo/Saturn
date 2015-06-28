#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_
#include "types.h"

#define INT_OR_FLT(X) \
	((X)->var->type == _INT ? (X)->var->val.INT : (X)->var->val.FLT)


void AddToEnv(Var *v);

/*********************************************************/
/*              Instruction Functions                    */
/*********************************************************/

void smov(Arg *dst, const Arg *src);

void sadd(Arg *dst, const Arg *src);

void ssub(Arg *dst, const Arg *src);

void smul(Arg *dst, const Arg *src);

void sdiv(Arg *dst, const Arg *src);

void srin(Arg *dst, const Arg *src);

void sout(Arg *dst, const Arg *src);

/* declaration instructions *
 * if src is null, the var  *
 * is not initialized       */
void sint(Arg *dst, const Arg *src);

void sflt(Arg *dst, const Arg *src);

void sstr(Arg *dst, const Arg *src);

/* These two only have the second parameter
 * so that they may be in the same array
 * of function pointers as the rest. All
 * calls to these two functions will have
 * NULL as the second argument. */
void sinc(Arg *dst, const Arg *src);
void sdec(Arg *dst, const Arg *src);

/*********************************************************/
/*                   Misc Functions                      */
/*********************************************************/

#endif
