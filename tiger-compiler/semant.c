#include "semant.h"
#include "allheader.h"
/* semant.c */

struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
	switch (a->kind)
	{
	case A_opExp:
		A_oper oper = a->u.op.oper;
		struct expty left = transExp(venv, tenv, a->u.op.left);
		struct expty right = transExp(venv, tenv, a->u.op.right);
		switch (oper)
		{
		case A_plusOp:
		case A_minusOp:
		case A_timesOp:
		case A_divideOp:
		case A_ltOp:
		case A_leOp:
		case A_gtOp:
		case A_geOp:
			if (left.ty->kind != Ty_int)
			{
				EM_error(a->u.op.left->pos, "integer required");
			}
			if (right.ty->kind != Ty_int)
			{
				EM_error(a->u.op.left->pos, "integer required");
			}
			return expTy(NULL, Ty_Int());
		case A_eqOp:
		case A_neqOp:
			if (left.ty->kind != Ty_int&&left.ty->kind != Ty_string&&left.ty->kind != Ty_array&&left.ty->kind != Ty_record&&left.ty->kind != Ty_nil)
			{
				EM_error(a->u.op.left->pos, "int, string, array, record required or unknown nil");
			}
			if (right.ty->kind != Ty_int&&right.ty->kind != Ty_string&&right.ty->kind != Ty_array&&right.ty->kind != Ty_record&&right.ty->kind != Ty_nil)
			{
				EM_error(a->u.op.right->pos, "int, string, array, record required or unknown nil");
			}
			if (left.ty->kind == Ty_nil&&right.ty->kind == Ty_nil)
			{
				EM_error(a->u.op.left->pos, "unknown nil");
			}
			return expTy(NULL, Ty_Int());
		default:
			assert(0);
		}
	}
}

struct expty transVar(S_table venv, S_table tenv, A_var v)
{
	switch (v->kind)
	{
	case A_simpleVar: {
		E_enventry x = S_look(venv, v->u.simple);

	}
	}
}

void SEM_transProg(A_exp exp)
{
	return;
}
