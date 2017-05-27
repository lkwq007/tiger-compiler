#pragma once
/* semant.h */
struct expty transVar(Tr_level level, Temp_label breakk, S_table venv, S_table tenv, A_var v);
struct expty transExp(Tr_level level, Temp_label breakk, S_table venv, S_table tenv, A_exp a);
Tr_exp transDec(Tr_level level, Temp_label breakk, S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);
typedef void *Tr_exp;

struct expty
{
	Tr_exp exp;
	Ty_ty ty;
};

F_fragList SEM_transProg(A_exp exp);