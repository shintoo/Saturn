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
	if (src != NULL) {
		if (src->var->type != _INT) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var->val.INT = src->var->type == _FLT ? src->var->val.FLT
			: src->var->val.INT;
	} else {
		dst->var->val.INT = 0;
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
	if (src) {
		if (src->var->type != _FLT && src->var->type != _INT) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var->val.FLT = src->var->type == _FLT ? src->var->val.FLT
			: src->var->val.INT;
	} else {
		dst->var->val.FLT = 0;
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
		dst->var->val.STR = malloc(strlen(src->var->val.STR) + 1);
		strcpy(dst->var->val.STR, src->var->val.STR);
	} else {
		dst->var = malloc(sizeof(Var));
		dst->var->val.STR = malloc(1);
		strcpy(dst->var->val.STR, "\0");
	}
	dst->var->type = _STR;
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);

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
			dst->var->val.INT = INT_OR_FLT(src); 
#ifdef DEBUG
			printf("[EXECUTE] Moved %d into variable \"%s\"\n",
			        src->var->val.INT, dst->var->label);
#endif
		break;
		case _FLT:
			dst->var->val.FLT = INT_OR_FLT(src);
#ifdef DEBUG
			printf("[EXECUTE] Moved %f into variable \"%s\"\n",
			        src->var->val.FLT, dst->var->label);
#endif
		break;
		case _STR:
			if (dst->var->val.STR = NULL) {
				dst->var->val.STR = malloc(strlen(src->var->val.STR));
			} else {
				dst->var->val.STR = realloc(
				    dst->var->val.STR,
				    strlen(src->var->val.STR)
				);
			}
			strcpy(dst->var->val.STR, src->var->val.STR);
#ifdef DEBUG
			printf("[EXECUTE] Moved \"%s\" into variable \"%s\"\n",
			        src->var->val.STR, dst->var->label);
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
		case _INT: fprintf(dst->var->val.FIL, "%d", src->var->val.INT); break;
		case _FLT: fprintf(dst->var->val.FIL, "%f", src->var->val.FLT); break;
		case _STR: fprintf(dst->var->val.FIL, "%s", src->var->val.STR); break;
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
	fgets(str, 32, src->var->val.FIL);
	*strchr(str, '\n') = '\0';

	switch(dst->var->type) {
		case _INT: dst->var->val.INT = atoi(str); break;
		case _FLT: dst->var->val.FLT = atof(str); break;
		case _STR:
			if (dst->var->val.STR != NULL) {
				free(dst->var->val.STR);
			}
			dst->var->val.STR = malloc(32);
			strcpy(dst->var->val.STR, str);
		break;
	}
}

void saturn_add(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Adding \"%s\" to \"%s\"\n", src->var->label, dst->var->label);
#endif
	if ((dst->var->type | src->var->type) > 1) {
		Abort("Error: mismatched types for ADD", "");
	}
	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}

	ARITHMETIC(dst, +=, src);
}

void saturn_sub(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Subtracting \"%s\" from \"%s\"\n",
		src->var->label, dst->var->label);
#endif

	if ((dst->var->type | src->var->type) > 1) {
		Abort("May only subtract numeric values", "");
	}

	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}

	ARITHMETIC(dst, -=, src);

}


void saturn_mul(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Multiplying \"%s\" by \"%s\"\n",
		dst->var->label, src->var->label);
#endif

	if ((dst->var->type | src->var->type) > 1) {
		Abort("May only multiply numeric values", "");
	}

	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}

	if (dst->var->type == _STR) {
		dst->var->val.STR = realloc(dst->var->val.STR, 
			strlen(dst->var->val.STR) + strlen(dst->var->val.STR) + 1);
		strcat(dst->var->val.STR, src->var->val.STR);
	} else {
		ARITHMETIC(dst, *=, src);
	}
}

void saturn_div(Arg *dst, const Arg *src) {
#ifdef DEBUG
	printf("[EXECUTE] Dividing \"%s\" by \"%s\"\n",
		dst->var->label, src->var->label);
#endif

	if ((dst->var->type | src->var->type) > 1) {
		Abort("May only divide numeric values", "");
	}

	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}

	if (dst->var->type == _STR) {
		dst->var->val.STR = realloc(dst->var->val.STR, 
			strlen(dst->var->val.STR) + strlen(dst->var->val.STR) + 1);
		strcat(dst->var->val.STR, src->var->val.STR);
	} else {
		if (INT_OR_FLT(src) == 0) {
			Abort("Dividing by 0: ", src->var->label);
		}
		ARITHMETIC(dst, /=, src);
	}
}

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
	if (src->var->type + dst->var->type != 4) {
		Abort("Error: mismatched types for CAT", "");
	}
	if (dst->var->isconst) {
		Abort("Error: constant variable: ", dst->var->label);
	}

	dst->var->val.STR = realloc(dst->var->val.STR, 
		strlen(dst->var->val.STR) + strlen(dst->var->val.STR) + 1);
	strcat(dst->var->val.STR, src->var->val.STR);
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
