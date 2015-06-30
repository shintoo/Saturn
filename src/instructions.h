#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_
#include "types.h"

#define INT_OR_FLT(X) \
	((X)->var->type == _INT ? (X)->var->val.INT : (X)->var->val.FLT)

#define ARITHMETIC(DST, OP, SRC) \
	switch ((DST)->var->type) { \
		case _INT: (DST)->var->val.INT OP INT_OR_FLT(SRC); break; \
		case _FLT: (DST)->var->val.FLT OP INT_OR_FLT(SRC); break; \
	}

void AddToEnv(Var *v);

/*********************************************************/
/*              Instruction Functions                    */
/*********************************************************/

// CHANGE ALL FUNCTIONS TO SATURN_X


void saturn_mov(Arg *dst, const Arg *src);

void saturn_add(Arg *dst, const Arg *src);

void saturn_cat(Arg *dst, const Arg *src);

void saturn_sub(Arg *dst, const Arg *src);

void saturn_mul(Arg *dst, const Arg *src);

void saturn_div(Arg *dst, const Arg *src);

void saturn_rin(Arg *dst, const Arg *src);

void saturn_out(Arg *dst, const Arg *src);

/* declaration instructions *
 * if src is null, the var  *
 * is not initialized       */
void saturn_int(Arg *dst, const Arg *src);

void saturn_flt(Arg *dst, const Arg *src);

void saturn_str(Arg *dst, const Arg *src);

/* These two only have the second parameter
 * so that they may be in the same array
 * of function pointers as the rest. All
 * calls to these two functions will have
 * NULL as the second argument. */
void saturn_inc(Arg *dst, const Arg *src);
void saturn_dec(Arg *dst, const Arg *src);

/*********************************************************/
/*                   Misc Functions                      */
/*********************************************************/

#endif
