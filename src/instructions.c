#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "parse.h"
#include "instructions.h"
#include "util.h"

#define __instruction_count  25

/* Holds variables */
extern Environment *env;

extern FILE *src_file;

/* The two rightmost bits serve as flags that are used
 * with the `cmp' command. They are set 00 for equal, 01
 * for less than, and 10 for greater than.
 */
extern char StatusWord;

/* Verify the arguments in a statement before execution */
static inline void check_args(int is_decl, const Statement *st) {
	if (st->argcount < 1) {
		ABORT("Error: No arguments");
	}
	if (st->args[0]->isliteral) {
		ABORT("Error: first operand may not be literal");
	}

	/* Statements that are not declarations */
	if (!is_decl) {
		if (!Env(st->args[0]->token)) {
			ABORT("Error: \'%s\' undeclared", st->args[0]->token);
		}
		if (st->args[1]) {
			if (!st->args[1]->isliteral) {
				if (!Env(st->args[1]->token)) {
					ABORT("Error: \'%s\' undeclared", st->args[1]->token);
				}
			}
		}
	}
}

/* Call the appropriate function indicated by the command member of the statement */
void Execute(const Statement *st) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Calling #%d\n", st->command);

	static void (*instructions[__instruction_count])(Arg *dst, const Arg *src) = {
		saturn_int, saturn_flt, saturn_str, saturn_fil, saturn_add, saturn_sub,
		saturn_mul, saturn_div, saturn_mod, saturn_inc, saturn_dec, saturn_mov,
		saturn_cat, saturn_get, saturn_out, saturn_opn, saturn_cls, saturn_cmp,
		saturn_jmp, saturn_jeq, saturn_jne, saturn_jig, saturn_jil, saturn_jge,
		saturn_jle
	};

	if (st->command > 17) {
		check_args(1, st);
	}
	if (st->command > 3 && st->command < 17) {
		check_args(0, st);
	} else {
		check_args(1, st);
	}
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Calling #%d\n", st->command);

	instructions[st->command](st->args[0], st->argcount == 2 ? st->args[1] : NULL);
}

/* Declares an integer variable */
void saturn_int(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Creating INT variable \"%s\"\n", dst->token);

	if (Env(dst->token)) {
		ABORT("Error: multiple declaration of %s", dst->token);
	}

	dst->var = malloc(sizeof(Var));
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);
	dst->var->type = _INT;
	dst->var->isconst = false;
	if (src) {
		if (src->var->type > 1) {
			ABORT("Mismatched type for initialization");
		}
		ARGVAL(dst, INT) = INT_OR_FLT(src);
	} else {
		ARGVAL(dst, INT) = 0;
	}
	
	AddToEnv(dst->var);
}

/* Declares a float variable */
void saturn_flt(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Creating FLT variable \"%s\"\n",
	    dst->token);

	if (Env(dst->token)) {
		ABORT("Error: multiple declaration of %s", dst->token);
	}

	dst->var = malloc(sizeof(Var));
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);
	dst->var->type = _FLT;
	dst->var->isconst = false;
	if (src) {
		if (src->var->type != _FLT && src->var->type != _INT) {
			ABORT("Mismatched type for initialization");
		}
		ARGVAL(dst, FLT) = src->var->type == _FLT ? ARGVAL(src, FLT)
			: ARGVAL(src, INT);
	} else {
		ARGVAL(dst, FLT) = 0;
	}

	AddToEnv(dst->var);
}

/* Declares a string variable */
void saturn_str(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Creating STR variable \"%s\"\n",
	    dst->token);

	if (src) {
		if (src->var->type != _STR) {
			ABORT("Mismatched type for initialization");
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

/* Declares a file variable */
void saturn_fil(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Creating FIL variable \"%s\"\n",
	    dst->token);

	if (Env(dst->token)) {
		ABORT("Error: multiple declaration");
	}
	if (!src) {
		ABORT("Error: must specify file path in declaration");
	}

	dst->var = malloc(sizeof(Var));
	dst->var->type = _FIL;
	dst->var->label = malloc(strlen(dst->token) + 1);
	strcpy(dst->var->label, dst->token);
	dst->var->isconst = false;
	dst->var->val.FIL.isopen = false;

	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Path to file: \"%s\"\n", src->token);
	dst->var->val.FIL.path = malloc(strlen(src->var->val.STR) + 1);
	strcpy(dst->var->val.FIL.path, src->token);	

	AddToEnv(dst->var);
}

/* Assigns the value of the second argument to the first */
void saturn_mov(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Destination: %s, source: %s\n",
	        dst->var->label, src->var->label);

	if ((src->var->type | dst->var->type) > 1 &&
			src->var->type + dst->var->type != 4) {
		ABORT("Illegal types for MOV: have %s and %s", TypeLabel(dst->var->type),
			TypeLabel(src->var->type));
	}

	switch (dst->var->type) {
		case _INT:
			ARGVAL(dst, INT) = INT_OR_FLT(src); 
			DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Moved %d into variable \"%s\"\n",
		        src->var->val.INT, dst->var->label);

		break;
		case _FLT:
			ARGVAL(dst, FLT) = INT_OR_FLT(src);
			DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Moved %f into variable \"%s\"\n",
			        src->var->val.FLT, dst->var->label);

		break;
		case _STR:
			ARGVAL(dst, STR) = realloc(
			    ARGVAL(dst, STR), strlen(ARGVAL(src, STR)) + 1);
			strcpy(ARGVAL(dst, STR), ARGVAL(src, STR));
			DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Moved \"%s\" into variable \"%s\"\n",
			        ARGVAL(src, STR), dst->var->label);

		break;
	}
}

/* Prints the second argument to the stream that is the first argument */
void saturn_out(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Writing variable \"%s\" to file \"%s\"\n", 
		src->var->label, dst->var->label);

	if (dst->var->type != _FIL) {
		ABORT("Error: first argument must be a FIL");
	}

	if (dst->var->val.FIL.isopen == false) {
		ABORT("Error: file not opened for IO");
	}

	switch(src->var->type) {
		case _INT: fprintf(ARGVAL(dst, FIL.pntr), "%d", ARGVAL(src, INT)); break;
		case _FLT: fprintf(ARGVAL(dst, FIL.pntr), "%g", ARGVAL(src, FLT)); break;
		case _STR: fprintf(ARGVAL(dst, FIL.pntr), "%s", ARGVAL(src, STR)); break;
	}
}

/* Reads a value into the first argument from the stream that is the second */
void saturn_get(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Reading into variable \"%s\" from file \"%s\"\n",
		dst->var->label, src->var->label);
	DEBUGMSG(_GREEN "INPUT: " _RESET);
	if (dst->var->isconst) {
		ABORT("Error: constant variable: %s", dst->var->label);
	}
	char str[32];
	fgets(str, 32, src->var->val.FIL.pntr);
	*strchr(str, '\n') = '\0';
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Fetched line \"%s\" from file \"%s\""
	         " at path \"%s\"\n", str, src->var->label, src->var->val.FIL.path);
	switch(dst->var->type) {
		case _INT: ARGVAL(dst, INT) = atoi(str); break;
		case _FLT: ARGVAL(dst, FLT) = atof(str); break;
		case _STR:
			if (ARGVAL(dst, STR)) {
				free(ARGVAL(dst, STR));
			}
			ARGVAL(dst, STR) = malloc(strlen(str) + 1);
			strcpy(ARGVAL(dst, STR), str);
		break;
	}
}

/* Check out instructions.h */
MAKE_ARITHMETIC_FUNCTION(add, +=);

MAKE_ARITHMETIC_FUNCTION(sub, -=);

MAKE_ARITHMETIC_FUNCTION(mul, *=);

MAKE_ARITHMETIC_FUNCTION(div, /=);

/* Must be explicitly defined because MAKE_ARITHMETIC_FUNCTION() allows
 * for float types 
 */
void saturn_mod(Arg *dst, const Arg *src) {
	if ((dst->var->type != _INT && src->var->type != _INT)) {
		ABORT("Error: MOD may only take INT types");
	}
	if (dst->var->isconst) {
		ABORT("Error: constant variable: %s", dst->var->label);
	}

	dst->var->val.INT %= src->var->val.INT;

}

/* Increments the dst argument by 1 */
void saturn_inc(Arg *dst, const Arg *src) {
	if (src != NULL) {
		ABORT("INC takes only one argument");
	}

	if (dst->var->type == _STR) {
		ABORT("INC takes only FLT or INT variables");
	}

	switch (dst->var->type) {
		case _INT: dst->var->val.INT++; break;
		case _FLT: dst->var->val.FLT++; break;
	}
}

/* Decrements the dst argument by 1 */
void saturn_dec(Arg *dst, const Arg *src) {
	if (src != NULL) {
		ABORT("DEC takes only one argument");
	}

	if (dst->var->type == _STR) {
		ABORT("DEC takes only FLT or INT variables");
	}

	switch (dst->var->type) {
		case _INT: dst->var->val.INT--; break;
		case _FLT: dst->var->val.FLT--; break;
	}
}

/* Concatenates the second argument onto the first, which is a string */
void saturn_cat(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Concatenating \"%s\" to \"%s\"\n",
		src->var->label, dst->var->label);
	int sz = 0;
	if (src->var->type != _STR) {
		ABORT("Error: First argument must be STR");
 	}
	if (dst->var->isconst) {
		ABORT("Error: constant variable: %s", dst->var->label);
	}
	sz = strlen(ARGVAL(dst, STR)) + strlen(ARGVAL(src, STR)) + 2;
	ARGVAL(dst, STR) = realloc(ARGVAL(dst, STR), sz);

	strncat(ARGVAL(dst, STR), ARGVAL(src, STR), sz);
}

/* Find the token for the second argument in the src_file and
 * set the file pointer to that location
 */
void saturn_jmp(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Searching for label \"%s\"\n", dst->token);
	fpos_t destination;
	char label[18];
	strncpy(label, dst->token, 16);
	strcat(label, ":");

	if ((FindLabel(label, &destination)) == 1) {
		DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Found label \"%s\". Jumping\n", label);
		fsetpos(src_file, &destination);
	} else {
		DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Label \"%s\" not found\n", label);
	}

}


/* instructions.h
 * Conditional jumps. The latter two numeric macro arguments
 * correspond to the two possible values of StatusWord.
 */
MAKE_COND_JMP(jeq, 0, 0);
MAKE_COND_JMP(jne, 1, 2);
MAKE_COND_JMP(jig, 2, 2);
MAKE_COND_JMP(jil, 1, 1);
MAKE_COND_JMP(jle, 1, 0);
MAKE_COND_JMP(jge, 2, 0);

/* Compare two arguments, set the StatusWord accordingly */
void saturn_cmp(Arg *dst, const Arg *src) {
	int strcmp_ret = 0;
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Comparing\n");
 	if ((src->var->type | dst->var->type) > 1 &&
			src->var->type + dst->var->type != 4) {
		ABORT(" Invalid types for cmp");
	}
	switch(dst->var->type) {
		case _INT:
		case _FLT:
			if (INT_OR_FLT(dst) > INT_OR_FLT(src)) {
				StatusWord = 2;
			} else if (INT_OR_FLT(dst) < INT_OR_FLT(src)) {
				StatusWord = 1;
			} else {
				StatusWord = 0;
			}
		break;
		case _STR:
			strcmp_ret = strcmp(ARGVAL(dst, STR), ARGVAL(src, STR));
			if (strcmp_ret > 0) {
				StatusWord = 2;
			} else if (strcmp_ret < 0) {
				StatusWord = 1;
			} else {
				StatusWord = 0;
			}
	}
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Set StatusWord to %d\n", StatusWord);
}

/* Opens a stream from a file variable */
void saturn_opn(Arg *dst, const Arg *src) {
	if (dst->var->type != _FIL) {
		ABORT("Error: `opn' takes type `fil'");
	}
	if (!src) {
		ABORT("Error: Must specify mode for `opn'");
	}
	if (src->var->type != _STR) {
		ABORT("Error: Mode for `opn' must be type `str'");
	}

	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Opening FIL variable \"%s\"\n"
	         "[" _YELLOW "EXECUTE" _RESET "]    Path: %s; Mode: %s\n",
              dst->var->label, dst->var->val.FIL.path, src->var->val.STR);

	dst->var->val.FIL.mode = src->var->val.STR;
	dst->var->val.FIL.pntr = fopen(dst->var->val.FIL.path, dst->var->val.FIL.mode);
	if (dst->var->val.FIL.pntr == NULL) {
		ABORT("Error: %s: No such file or directory", dst->var->val.FIL.path);
	}
	dst->var->val.FIL.isopen = true;
}

/* Closes a file */
void saturn_cls(Arg *dst, const Arg *src) {
	DEBUGMSG("[" _YELLOW "EXECUTE" _RESET "] Closing FIL variable\n");

	if (dst->var->type != _FIL) {
		ABORT("You can only cls files");
	}

	if (dst->var->val.FIL.isopen == false) {
		fprintf(stderr, "File not yet opened\n");
	}

	fclose(dst->var->val.FIL.pntr);
	dst->var->val.FIL.isopen = false;
}

/* Append a newly declared variable to the list of variables (environment) */
void AddToEnv(Var *v) {
	DEBUGMSG("[" _GREEN "  ENV  " _RESET "] Environment begins at %p\n", env->vars);
	DEBUGMSG("[" _GREEN "  ENV  " _RESET "] Adding %s to environment at %d\n",
	   v->label, env->varcount);

	if (env->varcount == env->memsize) {
		DEBUGMSG("[" _GREEN "  ENV  " _RESET "] Adding 10 memory blocks to environment\n");
		env->vars = realloc(env->vars, (env->memsize + 10) * sizeof(Var *));
		env->memsize += 10;
	}
	env->vars[env->varcount] = v;
	DEBUGMSG("[" _GREEN "  ENV  " _RESET "] %s added to environment at %d\n",
	        env->vars[env->varcount]->label, env->varcount);

	env->varcount++;
}
