#include "semant.h"
#include "allheader.h"
/* semant.c */
struct expty expTy(Tr_exp exp, Ty_ty ty)
{
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

struct expty transExpList(S_table venv, S_table tenv, A_expList list)
{
	if (list == NULL)
	{
		return expTy(NULL,NULL);
	}
	struct expty exp = transExp(venv, tenv, list->head);
	struct expty inner = transExpList(venv, tenv, list->tail);
	return expTy(NULL, Ty_TyList(exp.ty, inner.ty));
}

struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
	switch (a->kind)
	{
	case A_varExp:
	{
		struct expty exp;
		exp = transVar(venv, tenv, a->u.var);
		return exp;
		/* I am so dumb
		switch (var->kind)
		{
		case A_simpleVar:
		{
			binding = S_look(venv, var->u.simple);
			if (binding == NULL)
			{
				EM_error(a->pos, "Undefined var");
			}
			else
			{
				return expTy(NULL, binding->u.var.ty);
			}
			break;
		}
		case A_fieldVar:
		{
			exp=transExp()
			binding = S_look(venv, var->u.field.sym);
			
		}
		case A_subscriptVar:
		{
			binding = S_look(venv, var->u.subscript.var

		}
		default:
			break;
		}
		*/
	}
	case A_nilExp:
	{
		return expTy(NULL, Ty_Nil());
	}
	case A_intExp:
	{
		return expTy(NULL, Ty_Int());
	}
	case A_stringExp:
	{
		return expTy(NULL, Ty_String());
	}
	case A_callExp:
	{
		struct expty exp;
		A_expList list;
		Ty_tyList tylist;
		bool args_matched = TRUE;
		E_enventry binding = S_look(venv, a->u.call.func);
		//exp = transExpList(venv, tenv, a->u.call.args);
		for (list = a->u.call.args, tylist = binding->u.fun.formals; list&&tylist; list = list->tail, tylist = tylist->tail)
		{
			exp = transExp(venv, tenv, list->head);
			if (exp.ty!=tylist->head)
			{
				args_matched = FALSE;
				break;
			}
		}
		if (binding&&binding->kind == E_funEntry&&args_matched)
		{
			return expTy(NULL, binding->u.fun.result);
		}
		else
		{
			EM_error(a->pos, "Error function name or args types");
		}
		break;
	}
	case A_opExp:
	{
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
	case A_recordExp:
	{
		
	}
	case A_seqExp:
	{
		A_expList list;
		for (; list != NULL; list = list->tail)
		{
			
		}
	}
	case A_assignExp:
	{
		struct expty var = transVar(venv, tenv, a->u.assign.var);
		struct expty exp = transExp(venv, tenv, a->u.assign.exp);
		if (var.ty == exp.ty)
		{
			return expTy(NULL, Ty_Void());
		}
		else
		{
			EM_error(a->pos, "Error, assign type not matched");
		}
		break;
	}
	case A_ifExp:
	{
		struct expty test = transExp(venv, tenv, a->u.iff.test);
		struct expty then = transExp(venv, tenv, a->u.iff.then);
		struct expty elsee = transExp(venv, tenv, a->u.iff.elsee);
		if (test.ty->kind!=Ty_int)
		{
			EM_error(a->pos, "Error if test");
		}
		return expTy(NULL, Ty_Void());
	}
	case A_whileExp:
	{
		struct expty test = transExp(venv, tenv, a->u.whilee.test);
		struct expty body = transExp(venv, tenv, a->u.whilee.body);
		if (test.ty->kind != Ty_int)
		{
			EM_error(a->pos, "Error while test");
		}
		return expTy(NULL, Ty_Void());
	}
	case A_forExp:
	{
		struct expty test = transExp(venv, tenv, a->u.whilee.test);

	}
	case A_breakExp:
	{

	}
	case A_letExp:
	{
		struct expty exp;
		A_decList d;
		S_beginScope(venv);
		S_beginScope(tenv);
		for (d = a->u.let.decs; d; d = d->tail)
		{
			transDec(venv, tenv, d->head);
		}
		exp = transExp(venv, tenv, a->u.let.body);
		S_endScope(tenv);
		S_endScope(venv);
		return exp;
	}
	case A_arrayExp:
	{

	}
	}
	assert(0);
}

struct expty transVar(S_table venv, S_table tenv, A_var v)
{
	switch (v->kind)
	{
	case A_simpleVar: {
		E_enventry x = S_look(venv, v->u.simple);
		if (x&&x->kind == E_VarEntry)
		{
			return expTy(NULL, actual_ty(x->u.var.ty));
		}
		else
		{
			EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
			return expTy(NULL, Ty_Int());
		}
	}
	case A_fieldVar: {

	}
	}
	assert(0);
}

void transDec(S_table venv, S_table tenv, A_dec d)
{
	switch (d->kind)
	{
	case A_varDec:
	{
		struct expty e = transExp(venv, tenv, d->u.var.init);
		S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
		break;
	}
	case A_typeDec:
	{
		S_enter(tenv, d->u.type->head->name, transTy(tenv,d->u.type->head->ty));
		break;
	}
	case A_functionDec:
	{
		A_fundec f = d->u.function->head;
		Ty_ty resultTy = S_look(tenv, f->result);
		Ty_tyList formalTys = makeFormalTyList(tenv, f->params);
		S_enter(venv, f->name, E_FunEntry(formalTys, resultTy));
		S_beginScope(venv);
		{
			A_fieldList l;
			Ty_tyList t;
			for (l = f->params, t = formalTys; l; l = l->tail, t = t->tail)
			{
				S_enter(venv, l->head->name, E_VarEntry(t->head));
			}
		}
		//transExp(venv, tenv, d->u.function->body);
		break;
	}
	}
	return;
}

void SEM_transProg(A_exp exp)
{
	return;
}
