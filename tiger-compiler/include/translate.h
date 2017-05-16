#pragma once
/* translate.h */
typedef struct Tr_access *Tr_access;

typedef struct Tr_accessList_ *Tr_accessList;
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessLit tail);

Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);