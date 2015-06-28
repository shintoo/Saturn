#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"

extern Environment *env;

void (*instructions[11])(Arg *dst, const Arg *src);

void Execute(const Statement *st) {
	instructions[st->command](st->args[0], st->argcount == 2 ? st->args[1] : NULL);
}

void sint(Arg *dst, const Arg *src) {
	printf("[EXECUTE] Creating INT variable \"%s\"\n", dst->token);
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

void sflt(Arg *dst, const Arg *src) {
	printf("[EXECUTE] Creating FLT variable \"%s\"\n", dst->token);
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

void sstr(Arg *dst, const Arg *src) {
	printf("[EXECUTE] Creating STR variable \"%s\"\n", dst->token);
	if (src) {
		if (src->var->type != _STR) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var = malloc(sizeof(Var));
		dst->var->val.STR = malloc(strlen(src->var->val.STR) + 1);
		strcpy(dst->var->val.STR, src->var->val.STR);
	} else {
		dst->var = malloc(sizeof(Var));
		dst->var->val.STR = NULL;
	}
	dst->var->type = _STR;
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);

	AddToEnv(dst->var);

}

void smov(Arg *dst, const Arg *src) {
	printf("[EXECUTE] Destination: %s, source: %s\n",
	        dst->var->label, src->var->label);
	
	if ((dst->var->type | src->var->type) <= 1 || dst->var->type+src->var->type == 4) {
		Abort("Illegal types for MOV", "");
	}

	switch (dst->var->type) {
		case _INT:
			dst->var->val.INT = INT_OR_FLT(src); 
			printf("[EXECUTE] Moved %d into variable \"%s\"\n",
			        src->var->val.INT, dst->var->label);
		break;
		case _FLT:
			dst->var->val.FLT = INT_OR_FLT(src);
			printf("[EXECUTE] Moved %f into variable \"%s\"\n",
			        src->var->val.FLT, dst->var->label);
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
			printf("[EXECUTE] Moved \"%s\" into variable \"%s\"\n",
			        src->var->val.STR, dst->var->label);
		break;
	}
}

void sout(Arg *dst, const Arg *src) {
	printf("[EXECUTE] Writing variable \"%s\" to ...\n", src->var->label);
	if (dst->var->type != _FIL) {
		Abort("Error: first argument must be a FIL", "");
	}

	switch(src->var->type) {
		case _INT: fprintf(dst->var->val.FIL, "%d", src->var->val.INT); break;
		case _FLT: fprintf(dst->var->val.FIL, "%f", src->var->val.FLT); break;
		case _STR: fprintf(dst->var->val.FIL, "%s", src->var->val.STR); break;
	}
}

void sadd(Arg *dst, const Arg *src) {
	printf("[EXECUTE] Adding \"%s\" to \"%s\"\n", src->var->label, dst->var->label);
	if (dst->var->type != src->var->type) {
		Abort("Error: mismatched types for ADD", "");
	}

	switch(dst->var->type) {
		case _INT: dst->var->val.INT += src->var->val.INT; break;
		case _FLT: dst->var->val.FLT += src->var->val.FLT; break;
		case _STR:
			dst->var->val.STR = realloc(dst->var->val.STR, 
				strlen(dst->var->val.STR) + strlen(dst->var->val.STR) + 1);
			strcat(dst->var->val.STR, src->var->val.STR);
		break;
	}
}

void AddToEnv(Var *v) {
	printf("[ENVIRONMENT] Adding %s to environment at %d\n", v->label, env->varcount);

	if (env->varcount == env->memsize) {
		printf("[ENVIRONMENT] Adding 10 memory blocks to environment\n");
		env = realloc(env, env->memsize + 10);
		env->memsize += 10;
	}
	env->vars[env->varcount] = v;
	printf("[ENVIRONMENT] %s added to environment at %d\n",
	        env->vars[env->varcount]->label, env->varcount);
	env->varcount++;
}
