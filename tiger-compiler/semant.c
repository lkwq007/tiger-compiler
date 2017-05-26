/* semant.c */
#include "semant.h"
#include "allheader.h"
#include "absyn.h"
#define NULL ( (void *) 0)

// [del]assume that exp using actual Ty_ty[del], which can handle type & init exp, or two types
bool is_equal_ty(Ty_ty type, Ty_ty exp)
{
	type = actual_ty(type);
	exp = actual_ty(type);
	int tKind = type->kind;
	int eKind = exp->kind;
	// the same array or record type?
	if ((tKind == Ty_record || tKind == Ty_array) && type == exp)
	{
		return TRUE;
	}
	// record & nil
	if (tKind == Ty_record&&eKind == Ty_nil || tKind == Ty_nil&&eKind == Ty_record)
	{
		return TRUE;
	}
	// other case
	if (tKind != Ty_record&&tKind != Ty_array&&tKind == eKind)
	{
		return TRUE;
	}
	return FALSE;
}

// local function to skip past all the Names
Ty_ty actual_ty(Ty_ty dummy)
{
	if (dummy->kind == Ty_name)
	{
		return actual_ty(dummy->u.name.ty);
	}
	return dummy;
}

// lookup actual ty in tenv
Ty_ty S_look_ty(S_table tenv, S_symbol sym)
{
	Ty_ty type = S_look(tenv, sym);
	if (type)
	{
		return actual_ty(type);
	}
	return NULL;
}

// construct expty
struct expty expTy(Tr_exp exp, Ty_ty ty)
{
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

// 写完后发现没用了 transExpList
struct expty transExpList(S_table venv, S_table tenv, A_expList list)
{
	if (list == NULL)
	{
		return expTy(NULL, NULL);
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
		return transVar(venv, tenv, a->u.var);
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
		// finding func
		E_enventry binding = S_look(venv, a->u.call.func);
		if (binding&&binding->kind == E_funEntry)
		{
			// matching args
			for (list = a->u.call.args, tylist = binding->u.fun.formals; list&&tylist; list = list->tail, tylist = tylist->tail)
			{
				exp = transExp(venv, tenv, list->head);
				if (!is_equal_ty(exp.ty, tylist->head))
				{
					args_matched = FALSE;
					break;
				}
			}
			// same #args and same types
			if (args_matched&&tylist == NULL&&list == NULL)
			{
				// using actual return type
				return expTy(NULL, actual_ty(binding->u.fun.result));
			}
			else
			{
				EM_error(a->pos, "Error args types in %s", S_name(a->u.call.func));
			}
		}
		else
		{
			EM_error(a->pos, "Error function name %s", S_name(a->u.call.func));
		}
		return expTy(NULL, Ty_Int());
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
			if (!is_equal_ty(left.ty, right.ty))
			{
				EM_error(a->u.op.left->pos, "comparing var type not matched");
			}
			return expTy(NULL, Ty_Int());
		default:
			assert(0);
		}
	}
	case A_seqExp:
	{
		A_expList list = a->u.seq;
		struct expty exp;
		if (list)
		{
			for (; list != NULL; list = list->tail)
			{
				// Tr_seq? may using recursive
				exp = transExp(venv, tenv, list->head);
			}
			return exp;
		}
		else
		{
			return expTy(NULL, Ty_Void());
		}
	}
	case A_assignExp:
	{
		// var.ty cannot be nil
		struct expty var = transVar(venv, tenv, a->u.assign.var);
		struct expty exp = transExp(venv, tenv, a->u.assign.exp);
		if (is_equal_ty(var.ty, exp.ty))
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
		if (test.ty->kind != Ty_int)
		{
			EM_error(a->pos, "Error if test");
		}
		if (a->u.iff.elsee)
		{
			struct expty elsee = transExp(venv, tenv, a->u.iff.elsee);
			if (is_equal_ty(then.ty, elsee.ty))
			{
				return expTy(NULL, actual_ty(then.ty));
			}
			else
			{
				EM_error(a->pos, "then exp & else exp type not matched");
			}
		}
		if (then.ty->kind != Ty_void)
		{
			EM_error(a->pos, "if-else exp must have void value");
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
		if (body.ty->kind != Ty_void)
		{
			EM_error(a->pos, "while exp must have void value");
		}
		return expTy(NULL, Ty_Void());
	}
	case A_forExp:
	{
		struct expty lo = transExp(venv, tenv, a->u.forr.lo);
		struct expty hi = transExp(venv, tenv, a->u.forr.hi);
		struct expty body;
		if (lo.ty->kind != Ty_int || hi.ty->kind != Ty_int)
		{
			EM_error(a->pos, "range <lo hi> in for exp must be int");
		}
		S_beginScope(venv);
		S_enter(venv, a->u.forr.var, E_VarEntry(Ty_Int()));
		body = transExp(venv, tenv, a->u.forr.body);
		S_endScope(venv);
		if (body.ty->kind != Ty_void)
		{
			EM_error(a->pos, "the body of for exp must be void");
		}
		return expTy(NULL, Ty_Void());
	}
	case A_breakExp:
	{
		// how to deal with break?
		// we need extra args to record break
		return expTy(NULL, Ty_Void());
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
	case A_recordExp:
	{
		Ty_ty type = S_look_ty(tenv, a->u.record.typ);
		A_efieldList list = a->u.record.fields;
		bool field_matched = TRUE;
		if (type)
		{
			if (list)
			{
				Ty_fieldList tylist = type->u.record;
				struct expty exp;
				for (; list&&tylist; list = list->tail, tylist = tylist->tail)
				{
					if (list->head->name == tylist->head->name)
					{
						exp = transExp(venv, tenv, list->head->exp);
						if (!is_equal_ty(tylist->head->ty, exp.ty))
						{
							field_matched = FALSE;
							break;
						}
					}
				}
				if (field_matched&&tylist == NULL&&list == NULL)
				{
					return expTy(NULL, type);
				}
				else
				{
					EM_error(a->pos, "Record type and fields not match");
				}
			}
		}
		else
		{
			EM_error(a->pos, "Unknow record type %s", S_name(a->u.record.typ));
		}
		return expTy(NULL, Ty_Nil());
	}
	case A_arrayExp:
	{
		Ty_ty type = S_look_ty(tenv,a->u.array.typ);
		if (type&&type->kind == Ty_array)
		{
			struct expty size = transExp(venv, tenv, a->u.array.size);
			struct expty init = transExp(venv, tenv, a->u.array.init);
			if (size.ty->kind != Ty_int)
			{
				EM_error(a->u.array.size->pos, "size of array should be int");
			}
			if (is_equal_ty(type->u.array, init.ty))
			{
				return expTy(NULL, type);
			}
			EM_error(a->pos, "Array init type not matched");
		}
		else
		{
			EM_error(a->pos, "Unknow type %s", S_name(a->u.array.typ));
		}
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
		if (x&&x->kind == E_varEntry)
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
		bool is_exist = FALSE;
		struct expty field = transVar(venv, tenv, v->u.field.var);
		if (field.ty->kind != Ty_record)
		{
			EM_error(v->u.field.var->pos, "variable with field but not a record");
		}
		Ty_fieldList list;
		for (list = field.ty->u.record; list; list = list->tail)
		{
			if (list->head->name == v->u.field.sym)
			{
				is_exist = TRUE;
				break;
			}
		}
		if (!is_exist)
		{
			EM_error(v->pos, "record not contain this field");
			return expTy(NULL, Ty_Int());
		}
		return expTy(NULL, list->head->ty);
	}
	case A_subscriptVar: {
		struct expty ptr = transVar(venv, tenv, v->u.subscript);
		struct expty exp = transExp(venv, tenv, v->u.subscript.exp);
		if (ptr.ty->kind != Ty_array)
		{
			EM_error(v->pos, "variable with subscript is not an array");
			return expTy(NULL, Ty_Int());
		}
		if (exp.ty->kind != Ty_int)
		{
			EM_error(v->u.subscript.exp->pos, "subscript should be interger");
		}
		return expTy(NULL, ptr.ty->u.array);
	}
	}
	assert(0);
}

// return Ty_ty from A_ty
Ty_ty transTy(S_table tenv, A_ty a)
{
	switch (a->kind)
	{
	case A_nameTy:
	case A_recordTy:
	case A_arrayTy:
	default:
		break;
	}
	assert(0);
}

void transDec(S_table venv, S_table tenv, A_dec d)
{
	// 认为 d 是已经被完整构建的声明，解析时构建
	switch (d->kind)
	{
	case A_varDec:
	{
		struct expty e = transExp(venv, tenv, d->u.var.init);
		// 初始化的类型检查我还没有太好的思路
		if (d->u.var.typ != NULL)
		{
			Ty_ty type = S_look(tenv, d->u.var.typ);
			if ((e.ty->kind == Ty_nil&&type->kind != Ty_record) || type->kind != e.ty->kind)
			{
				EM_error(d->pos, "Init not compatible");
			}
			S_enter(venv, d->u.var.var, E_VarEntry(type));
		}
		else
		{
			S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
		}
		break;
	}
	// Todo 递归的类型定义处理
	case A_typeDec:
	{
		S_enter(tenv, d->u.type->head->name, transTy(tenv, d->u.type->head->ty));
		break;
	}
	// Todo 递归函数以及多函数处理
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
