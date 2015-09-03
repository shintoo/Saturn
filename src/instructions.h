#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_
#include "types.h"

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

/* finish this */
#define MAKE_COND_JMP(NAME, SW1, SW2) \
void saturn_##NAME(Arg *dst, const Arg *src) { \
	if (StatusWord == SW1 || StatusWord == SW2) { \
		saturn_jmp(dst, src); \
	} \
}

/* AddToEnv
 * Adds a variable to the environment
 * This function is called when a declaration
 * statement is executed, adding the label of
 * the variable to the environment for use
 * in later statements
 */
void AddToEnv(Var *v);

/*********************************************************/
/*              Instruction Functions                    */
/*********************************************************/

void saturn_mov(Arg *dst, const Arg *src);

void saturn_add(Arg *dst, const Arg *src);

void saturn_cat(Arg *dst, const Arg *src);

void saturn_sub(Arg *dst, const Arg *src);

void saturn_mul(Arg *dst, const Arg *src);

void saturn_div(Arg *dst, const Arg *src);

void saturn_mod(Arg *dst, const Arg *src);

void saturn_get(Arg *dst, const Arg *src);

void saturn_out(Arg *dst, const Arg *src);

void saturn_fil(Arg *dst, const Arg *src);

void saturn_opn(Arg *dst, const Arg *src);

void saturn_cls(Arg *dst, const Arg *src);

void saturn_cmp(Arg *dst, const Arg *src);

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

void saturn_jmp(Arg *dst, const Arg *src);

void saturn_jeq(Arg *dst, const Arg *src);
void saturn_jne(Arg *dst, const Arg *src);
void saturn_jig(Arg *dst, const Arg *src);
void saturn_jge(Arg *dst, const Arg *src);
void saturn_jil(Arg *dst, const Arg *src);
void saturn_jle(Arg *dst, const Arg *src);

/*********************************************************/
/*                   Misc Functions                      */
/*********************************************************/

#endif
