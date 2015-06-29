#ifndef _UTIL_H_
#define _UTIL_H_

#define OUT_STDOUT_NEWLINE(X) \
	(X)->command == OUT ? (X)->args[1]->var->type == _STR ? (X)->args[1]->var->val.STR[0] == '\n' ? 1 : 0 : 0 : 0



#define INT_OR_FLT(X) \
	((X)->var->type == _INT ? (X)->var->val.INT : (X)->var->val.FLT)

#define ARITHMETIC(DST, OP, SRC) \
	switch ((DST)->var->type) { \
		case _INT: (DST)->var->val.INT OP INT_OR_FLT(SRC); break; \
		case _FLT: (DST)->var->val.FLT OP INT_OR_FLT(SRC); break; \
	}

#endif
