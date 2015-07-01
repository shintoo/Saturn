#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

extern Environment *env;

void (*instructions[13])(Arg *dst, const Arg *src);

void Execute(const Statement *st) {
	instructions[st->command](st->args[0], st->argcount == 2 ? st->args[1] : NULL);
}

void saturn_int(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Creating INT variable \"%s\"\n", dst->token);
#endif
	if (Env(dst->token)) {
		Abort("Error: multiple declaration of ", dst->token);
	}

	dst->var = malloc(sizeof(Var));
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);
	dst->var->type = _INT;
	dst->var->isconst = false;
	if (src != NULL) {
		if (src->var->type != _INT) {
			Abort("Mismatched type for initialization", "");
		}
		ARGVAL(dst, INT) = src->var->type == _FLT ? ARGVAL(src, FLT)
			: ARGVAL(src, INT);
	} else {
		ARGVAL(dst, INT) = 0;
	}
	
	AddToEnv(dst->var);
}

void saturn_flt(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Creating FLT variable \"%s\"\n", dst->token);
#endif
	dst->var = malloc(sizeof(Var));
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);
	dst->var->type = _FLT;
	dst->var->isconst = false;
	if (src) {
		if (src->var->type != _FLT && src->var->type != _INT) {
			Abort("Mismatched type for initialization", "");
		}
		ARGVAL(dst, FLT) = src->var->type == _FLT ? ARGVAL(src, FLT)
			: ARGVAL(src, INT);
	} else {
		ARGVAL(dst, FLT) = 0;
	}

	AddToEnv(dst->var);
}

void saturn_str(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Creating STR variable \"%s\"\n", dst->token);
#endif
	if (src) {
		if (src->var->type != _STR) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var = malloc(sizeof(Var));
		ARGVAL(dst, STR) = malloc(strlen(src->var->val.STR) + 1);
		strcpy(ARGVAL(dst, STR), ARGVAL(src, STR));
	} else {
		dst->var = malloc(sizeof(Var));
		ARGVAL(dst, STR) = malloc(1);
		ARGVAL(dst, STR)[0] = '\0';
	}
	dst->var->type = _STR;
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);
	dst->var->isconst = false;
	AddToEnv(dst->var);

}
/*
void sfil(Arg *dst, const Arg *src) {
	

*/
void saturn_mov(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Destination: %s, source: %s\n",
	        dst->var->label, src->var->label);
#endif
	if ((src->var->type | dst->var->type) > 1 || src->var->type + dst->var->type == 4) {
		Abort("Illegal types for MOV", "");
	}

	switch (dst->var->type) {
		case _INT:
			ARGVAL(dst, INT) = INT_OR_FLT(src); 
#ifdef DEBUG
			printf("[EXECUTE] Moved %d into variable \"%s\"\n",
			        src->var->val.INT, dst->var->label);
#endif
		break;
		case _FLT:
			ARGVAL(dst, FLT) = INT_OR_FLT(src);
#ifdef DEBUG
			printf("[EXECUTE] Moved %f into variable \"%s\"\n",
			        src->var->val.FLT, dst->var->label);
#endif
		break;
		case _STR:
			if (ARGVAL(dst, STR) = NULL) {
				ARGVAL(dst, STR) = malloc(strlen(ARGVAL(src, STR)));
			} else {
				ARGVAL(dst, STR) = realloc(
				    ARGVAL(dst, STR),
				    strlen(ARGVAL(src, STR))
				);
			}
			strcpy(ARGVAL(dst, STR), ARGVAL(src, STR));
#ifdef DEBUG
			printf("[EXECUTE] Moved \"%s\" into variable \"%s\"\n",
			        ARGVAL(src, STR), dst->var->label);
#endif
		break;
	}
}

void saturn_out(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Writing variable \"%s\" to file \"%s\"\n", 
		src->var->label, dst->var->label);
#endif
	if (dst->var->type != _FIL) {
		Abort("Error: first argument must be a FIL", "");
	}

	switch(src->var->type) {
		case _INT: fprintf(ARGVAL(dst, FIL), "%d", ARGVAL(src, INT)); break;
		case _FLT: fprintf(ARGVAL(dst, FIL), "%f", ARGVAL(src, FLT)); break;
		case _STR: fprintf(ARGVAL(dst, FIL), "%s", ARGVAL(src, STR)); break;
	}
}

void saturn_rin(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Reading into variable \"%s\" from file \"%s\"\n",
		dst->var->label, src->var->label);
	printf("INPUT: ");
#endif
	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}
	char str[32];
	fgets(str, 32, ARGVAL(src, FIL));
	*strchr(str, '\n') = '\0';

	switch(dst->var->type) {
		case _INT: ARGVAL(dst, INT) = atoi(str); break;
		case _FLT: ARGVAL(dst, FLT) = atof(str); break;
		case _STR:
			if (ARGVAL(dst, STR) != NULL) {
				free(ARGVAL(dst, STR));
			}
			ARGVAL(dst, STR) = malloc(strlen(str) + 1);
			strcpy(ARGVAL(dst, STR), str);
		break;
	}
}

MAKE_ARITHMETIC_FUNCTION(add, +=)

MAKE_ARITHMETIC_FUNCTION(sub, -=)

MAKE_ARITHMETIC_FUNCTION(mul, *=)

MAKE_ARITHMETIC_FUNCTION(div, /=)

void saturn_inc(Arg *dst, const Arg *src) {
	if (src != NULL) {
		Abort("INC takes only one argument", "");
	}

	if (dst->var->type == _STR) {
		Abort("INC takes only FLT or INT variables", "");
	}

	switch (dst->var->type) {
		case _INT: dst->var->val.INT++; break;
		case _FLT: dst->var->val.FLT++; break;
	}
}

void saturn_dec(Arg *dst, const Arg *src) {
	if (src != NULL) {
		Abort("DEC takes only one argument", "");
	}

	if (dst->var->type == _STR) {
		Abort("DEC takes only FLT or INT variables", "");
	}

	switch (dst->var->type) {
		case _INT: dst->var->val.INT--; break;
		case _FLT: dst->var->val.FLT--; break;
	}
}

void saturn_cat(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Concatenating \"%s\" to \"%s\"\n", src->var->label, dst->var->label);
#endif
	if (src->var->type != _STR) {
		Abort("Error: First argument must be STR", "");
	}
	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}
/*
	switch(src->var->type) {
		case _INT: 
*/
//	if (sizeof(dst->var->val.STR) 
	if (sizeof(ARGVAL(dst, STR))
		< sizeof(ARGVAL(dst, STR)) + strlen(ARGVAL(src, STR))) {
		dst->var->val.STR = realloc(ARGVAL(dst, STR), 
			strlen(ARGVAL(src, STR)) + strlen(ARGVAL(dst, STR)) + 2);
	}
	strcat(ARGVAL(dst, STR), ARGVAL(src, STR));
}


void AddToEnv(Var *v) {
#ifdef DEBUG
	printf("[ENVIRONMENT] Adding %s to environment at %d\n", v->label, env->varcount);
#endif
	if (env->varcount == env->memsize) {
		printf("[ENVIRONMENT] Adding 10 memory blocks to environment\n");
		env = realloc(env, env->memsize + 10);
		env->memsize += 10;
	}
	env->vars[env->varcount] = v;
#ifdef DEBUG
	printf("[ENVIRONMENT] %s added to environment at %d\n",
	        env->vars[env->varcount]->label, env->varcount);
#endif
	env->varcount++;
}
