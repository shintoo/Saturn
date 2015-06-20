#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"

extern Environment *env;

void (*instructions[11])(Arg *dst, const Arg *src);

void Execute(const Statement *st) {
	instructions[st->command](st->args[0], st->args[1]);
}

void sint(Arg *dst, const Arg *src) {
	dst->var = malloc(sizeof(Var));
	dst->var->label = dst->token;
	dst->var->type = _INT;
	if (src) {
		if (src->var->type != _INT) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var->val.INT = src->var->val.INT;
	} else {
		dst->var->val.INT = 0;
	}
	
	AddToEnv(dst->var);

}

void sflt(Arg *dst, const Arg *src) {
	dst->var = malloc(sizeof(Var));
	dst->var->label = dst->token;
	dst->var->type = _FLT;
	if (src) {
		if (src->var->type != _FLT) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var->val.FLT = src->var->val.FLT;
	} else {
		dst->var->val.FLT = src->var->val.FLT;
	}

	AddToEnv(dst->var);

}

void sstr(Arg *dst, const Arg *src) {
	if (src) {
		if (src->var->type != _STR) {
			Abort("Mismatched type for initialization", "");
		}
		dst->var = malloc(sizeof(Var));
		if (dst->var->val.STR != NULL) {
			free(dst->var->val.STR);
		}
		dst->var->val.STR = malloc(strlen(src->var->val.STR));
		strcpy(dst->var->val.STR, src->var->val.STR);
	} else {
		dst->var = malloc(sizeof(Var));
		dst->var->val.STR = NULL;
	}
	dst->var->type = _STR;
	dst->var->label = dst->token;
	AddToEnv(dst->var);

}

void smov(Arg *dst, const Arg *src) {
	if (dst->var->type != src->var->type) {
		Abort("Error: mismatched types", "");
	}

	switch (dst->var->type) {
		case _INT: dst->var->val.INT = src->var->val.INT; break;
		case _FLT: dst->var->val.FLT = src->var->val.FLT; break;
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
		break;
	}
}

void sout(Arg *dst, const Arg *src) {
	if (dst->var->type != _FIL) {
		Abort("Error: first argument must be a FIL", "");
	}

	switch(src->var->type) {
		case _INT: fprintf(dst->var->val.FIL, "%d", src->var->val.INT); break;
		case _FLT: fprintf(dst->var->val.FIL, "%f", src->var->val.FLT); break;
		case _STR: fprintf(dst->var->val.FIL, "%s", src->var->val.STR); break;
	}
}

void AddToEnv(Var *v) {
	printf("ADDING %s TO ENVIRONMENT AT %d\n", v->label, env->varcount);
	
	if (env->varcount == env->memsize) {
		printf("ADDING MEMORY TO ENVIRONMENT\n");
		env = realloc(env, env->memsize + 10);
		env->memsize += 10;
	}
	env->vars[env->varcount] = v;
	printf("%s ADDED TO ENVIRONMENT AT %d\n", env->vars[env->varcount]->label,
	       env->varcount);
	env->varcount++;
}
