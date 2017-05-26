#pragma once
/* translate.h */
#include "temp.h"
#include "frame.h"

typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;
typedef struct Tr_level_ *Tr_level;
typedef struct patchList_ *patchList;
typedef struct Tr_exp_ *Tr_exp;
typedef struct Tr_expList_ *Tr_expList;


// Tr_exp Tr_simpleVar(Tr_access, );
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);






Tr_exp Tr_numExp(int num);
Tr_exp Tr_stringExp(string str);
Tr_exp Tr_callExp(Tr_level level, Temp_label label, Tr_expList argList);
Tr_exp Tr_opExp(int op, Tr_exp left, Tr_exp right);
Tr_exp Tr_relExp(int op, Tr_exp left, Tr_exp right);
Tr_exp Tr_stringRelExp(int op, Tr_exp left, Tr_exp right);
Tr_exp Tr_recordExp(Tr_expList fields, int size);
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);
Tr_exp Tr_seqExp(Tr_expList stmts);
Tr_exp Tr_ifExp(Tr_exp cond, Tr_exp then, Tr_exp else_);
Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp body);
Tr_exp Tr_forExp(Tr_access access,
                      Tr_exp low,
                      Tr_exp high,
                      Tr_exp body);
Tr_exp Tr_assignExp(Tr_exp lhs, Tr_exp rhs);

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level);
Tr_exp Tr_fieldVar(Tr_exp record, int index);

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals);
F_fragList Tr_getResult(void);