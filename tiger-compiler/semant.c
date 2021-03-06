/* semant.c */
#include "semant.h"
#include "translate.h"
#include "env.h"
#include "types.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "util.h"
#include <assert.h>
#include <stdlib.h>
#include "printtree.h"
static Ty_ty actual_ty(Ty_ty dummy);
// [del]assume that exp using actual Ty_ty[del], which can handle type & init exp, or two types
static bool is_equal_ty(Ty_ty type, Ty_ty exp)
{
	type = actual_ty(type);
	exp = actual_ty(exp);
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
static Ty_ty actual_ty(Ty_ty dummy)
{
	// skip the alias name
	if (dummy&&dummy->kind == Ty_name)
	{
		return actual_ty(dummy->u.name.ty);
	}
	return dummy;
}

// lookup actual ty in tenv
// in some cases, e.g. type dec, we should not use this function, as the actual type may not exist
static Ty_ty S_look_ty(S_table tenv, S_symbol sym)
{
	Ty_ty type = S_look(tenv, sym);
	if (type)
	{
		return actual_ty(type);
	}
	return NULL;
}

// error checking in makelist
// make formal tylist from function args
static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params)
{
	Ty_ty temp;
	if (params == NULL)
	{
		return NULL;
	}
	temp = S_look_ty(tenv, params->head->typ);
	if (temp == NULL)
	{
		// won't return NULL ty
		EM_error(params->head->pos, "Undefined type <%s> of formal \"%s\"", S_name(params->head->typ), S_name(params->head->name));
		// return int type
		temp = Ty_Int();
	}
	return Ty_TyList(temp, makeFormalTyList(tenv, params->tail));
}

// make field tylist from abstract record field list
static Ty_fieldList makeFieldList(S_table tenv, A_fieldList record)
{
	Ty_ty temp;
	S_symbol name;
	if (record == NULL)
	{
		return NULL;
	}
	temp = S_look(tenv, record->head->typ);
	if (temp == NULL)
	{
		// won't return NULL ty
		EM_error(record->head->pos, "Undefined type <%s> of field \"%s\"", S_name(record->head->typ), S_name(record->head->name));
		temp = Ty_Int();
		name = S_Symbol("error");
	}
	else
	{
		name = record->head->name;
	}
	return Ty_FieldList(Ty_Field(name, temp), makeFieldList(tenv, record->tail));
}

// make escape list from params assume all escape
static U_boolList makeFormalList(A_fieldList params)
{
	if (params == NULL)
	{
		return NULL;
	}
	return U_BoolList(TRUE, makeFormalList(params->tail));
}

// construct expty
static struct expty expTy(Tr_exp exp, Ty_ty ty)
{
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

// return exp in Tr_exp with its type from A_exp
struct expty transExp(Tr_level level, Temp_label breakk, S_table venv, S_table tenv, A_exp a)
{
	switch (a->kind)
	{
	case A_varExp:
	{
		return transVar(level, breakk, venv, tenv, a->u.var);
	}
	case A_nilExp:
	{
		return expTy(Tr_noExp(), Ty_Nil());
	}
	case A_intExp:
	{
		return expTy(Tr_numExp(a->u.intt), Ty_Int());
	}
	case A_stringExp:
	{
		return expTy(Tr_stringExp(a->u.stringg), Ty_String());
	}
	case A_callExp:
	{
		struct expty exp;
		A_expList list;
		Ty_tyList tylist;
		bool args_matched = TRUE;
		// finding func
		E_enventry binding = S_look(venv, a->u.call.func);
		Tr_expList elist, tmplist;
		elist = NULL;
		tmplist = elist;
		if (binding&&binding->kind == E_funEntry)
		{
			// matching args
			for (list = a->u.call.args, tylist = binding->u.fun.formals; list&&tylist; list = list->tail, tylist = tylist->tail)
			{
				exp = transExp(level, breakk, venv, tenv, list->head);
				if (tmplist == NULL)
				{
					elist = Tr_ExpList(exp.exp, NULL);
					tmplist = elist;
				}
				else
				{
					tmplist->tail = Tr_ExpList(exp.exp, NULL);
					tmplist = tmplist->tail;
				}
				if (!is_equal_ty(exp.ty, tylist->head))
				{
					args_matched = FALSE;
				}
			}
			// same #args and same types
			if (args_matched&&tylist == NULL&&list == NULL)
			{
				// using actual return type
				return expTy(Tr_callExp(binding->u.fun.level, binding->u.fun.label, elist), actual_ty(binding->u.fun.result));
			}
			else
			{
				// TODO, full type msg in error
				EM_error(a->pos, "Error: This function \"%s\" is applied with mismatched arguments", S_name(a->u.call.func));
			}
		}
		else
		{
			EM_error(a->pos, "Error: Unbound function \"%s\"", S_name(a->u.call.func));
		}
		return expTy(Tr_noExp(), Ty_Void());
	}
	case A_opExp:
	{
		A_oper oper = a->u.op.oper;
		struct expty left = transExp(level, breakk, venv, tenv, a->u.op.left);
		struct expty right = transExp(level, breakk, venv, tenv, a->u.op.right);
		switch (oper)
		{
		case A_plusOp:
		case A_minusOp:
		case A_timesOp:
		case A_divideOp:
		{
			if (left.ty->kind != Ty_int)
			{
				EM_error(a->u.op.left->pos, "Error: This expression was expected of type int");
			}
			if (right.ty->kind != Ty_int)
			{
				EM_error(a->u.op.right->pos, "Error: This expression was expected of type int");
			}
			return expTy(Tr_opExp(oper, left.exp, right.exp), Ty_Int());
		}
		case A_ltOp:
		case A_leOp:
		case A_gtOp:
		case A_geOp:
		{
			if (left.ty->kind != Ty_int)
			{
				EM_error(a->u.op.left->pos, "This expression was expected of type int");
			}
			if (right.ty->kind != Ty_int)
			{
				EM_error(a->u.op.right->pos, "This expression was expected of type int");
			}
			break;
		}
		case A_eqOp:
		case A_neqOp:
		{
			if (left.ty->kind != Ty_int&&left.ty->kind != Ty_string&&left.ty->kind != Ty_array&&left.ty->kind != Ty_record&&left.ty->kind != Ty_nil)
			{
				EM_error(a->u.op.left->pos, "Error: This expression can not be compared, only int, string, array, record or nil can be compared");
			}
			if (right.ty->kind != Ty_int&&right.ty->kind != Ty_string&&right.ty->kind != Ty_array&&right.ty->kind != Ty_record&&right.ty->kind != Ty_nil)
			{
				EM_error(a->u.op.right->pos, "Error: This expression can not be compared, only int, string, array, record or nil can be compared");
			}
			if (left.ty->kind == Ty_nil&&right.ty->kind == Ty_nil)
			{
				EM_error(a->pos, "Error: Can not compare nil with nil");
			}
			if (!is_equal_ty(left.ty, right.ty))
			{
				EM_error(a->pos, "Error: Can not compare expressions with different types");
			}
			break;
		}
		default:
			assert(0);
		}
		if (left.ty->kind == Ty_string)
		{
			// compare string
			return expTy(Tr_stringRelExp(oper, left.exp, right.exp), Ty_Int());
		}
		// 1-true, 0-false, returning int
		return expTy(Tr_relExp(oper, left.exp, right.exp), Ty_Int());
	}
	case A_seqExp:
	{
		A_expList list = a->u.seq;
		struct expty exp;
		Tr_expList trlist = NULL, temp;
		if (list)
		{
			for (; list != NULL; list = list->tail)
			{
				exp = transExp(level, breakk, venv, tenv, list->head);
				if (trlist == NULL)
				{
					trlist = Tr_ExpList(exp.exp, NULL);
					temp = trlist;
				}
				else
				{
					temp->tail = Tr_ExpList(exp.exp, NULL);
					temp = temp->tail;
				}
			}
			// using actual ty
			return expTy(Tr_seqExp(trlist), actual_ty(exp.ty));
		}
		else
		{
			// empty seq
			return expTy(Tr_noExp(), Ty_Void());
		}
	}
	case A_assignExp:
	{
		// var.ty cannot be nil
		struct expty var = transVar(level, breakk, venv, tenv, a->u.assign.var);
		struct expty exp = transExp(level, breakk, venv, tenv, a->u.assign.exp);
		if (is_equal_ty(var.ty, exp.ty))
		{
			return expTy(Tr_assignExp(var.exp, exp.exp), Ty_Void());
		}
		else
		{

			EM_error(a->pos, "Error: Assignment type mismatch");
		}
		return expTy(Tr_noExp(), Ty_Void());
	}
	case A_ifExp:
	{
		struct expty test = transExp(level, breakk, venv, tenv, a->u.iff.test);
		struct expty then = transExp(level, breakk, venv, tenv, a->u.iff.then);
		if (test.ty->kind != Ty_int)
		{
			EM_error(a->u.iff.test->pos, "Error: This testing expression was expected of type int");
		}
		if (a->u.iff.elsee)
		{
			struct expty elsee = transExp(level, breakk, venv, tenv, a->u.iff.elsee);
			if (is_equal_ty(then.ty, elsee.ty))
			{
				return expTy(Tr_ifExp(test.exp, then.exp, elsee.exp), actual_ty(then.ty));
			}
			else
			{
				EM_error(a->u.iff.then->pos, "Error: Two branches of if-else expression should have the same type");
			}
		}
		if (then.ty->kind != Ty_void)
		{
			EM_error(a->u.iff.then->pos, "Error: if-then expression must have void value");
		}
		return expTy(Tr_ifExp(test.exp, then.exp, Tr_noExp()), Ty_Void());
	}
	case A_whileExp:
	{
		// allocate an end label for the `break` exp jumping to
		// for convenience, we use a label to achieve that, it dose do harm to the abstract interface
		Temp_label end = Temp_newlabel();
		struct expty test = transExp(level, breakk, venv, tenv, a->u.whilee.test);
		struct expty body = transExp(level, end, venv, tenv, a->u.whilee.body);
		if (test.ty->kind != Ty_int)
		{
			EM_error(a->u.whilee.test->pos, "Error: This testing expression was expected of type int");
		}
		if (body.ty->kind != Ty_void)
		{
			EM_error(a->u.whilee.body->pos, "Error: This expression must have void value");
		}
		return expTy(Tr_whileExp(test.exp, end, body.exp), Ty_Void());
	}
	case A_forExp:
	{
		Temp_label end = Temp_newlabel();
		Tr_access access;
		struct expty lo = transExp(level, breakk, venv, tenv, a->u.forr.lo);
		struct expty hi = transExp(level, breakk, venv, tenv, a->u.forr.hi);
		struct expty body;
		if (lo.ty->kind != Ty_int || hi.ty->kind != Ty_int)
		{
			EM_error(a->u.forr.lo->pos, "Error: Range lo-hi in for exp must have int value");
		}
		S_beginScope(venv);
		access = Tr_allocLocal(level, a->u.forr.escape);
		S_enter(venv, a->u.forr.var, E_VarEntry(access, Ty_Int()));
		body = transExp(level, end, venv, tenv, a->u.forr.body);
		S_endScope(venv);
		if (body.ty->kind != Ty_void)
		{
			EM_error(a->u.forr.body->pos, "Error: This expression must have void value");
		}
		return expTy(Tr_forExp(access, end, lo.exp, hi.exp, body.exp), Ty_Void());
	}
	case A_breakExp:
	{
		// using breakk to record while and for end label
		if (breakk == NULL)
		{
			EM_error(a->pos, "Error: break expression should be nested in the body of while or for expressions");
			return expTy(Tr_noExp(), Ty_Void());
		}
		return expTy(Tr_breakExp(breakk), Ty_Void());
	}
	case A_letExp:
	{
		struct expty exp;
		Tr_expList list = NULL, temp = list;
		Tr_exp dec;
		A_decList d;
		S_beginScope(venv);
		S_beginScope(tenv);
		for (d = a->u.let.decs; d; d = d->tail)
		{
			dec = transDec(level, breakk, venv, tenv, d->head);
			if (dec)
			{
				if (list == NULL)
				{
					list = Tr_ExpList(dec, NULL);
					temp = list;
				}
				else
				{
					temp->tail = Tr_ExpList(dec, NULL);
					temp = temp->tail;
				}
			}
		}
		exp = transExp(level, breakk, venv, tenv, a->u.let.body);
		if (temp)
		{
			temp->tail = Tr_ExpList(exp.exp, NULL);
		}
		else
		{
			list = Tr_ExpList(exp.exp, NULL);
		}
		S_endScope(tenv);
		S_endScope(venv);
		return expTy(Tr_seqExp(list), actual_ty(exp.ty));
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
				int size = 0;
				Ty_fieldList tylist = type->u.record;
				Tr_expList trlist = NULL, temp = trlist;
				struct expty exp;
				for (; list&&tylist; list = list->tail, tylist = tylist->tail)
				{
					if (list->head->name == tylist->head->name)
					{
						exp = transExp(level, breakk, venv, tenv, list->head->exp);
						if (!is_equal_ty(tylist->head->ty, exp.ty))
						{
							field_matched = FALSE;
						}
						if (temp == NULL)
						{
							trlist = Tr_ExpList(exp.exp, NULL);
							temp = trlist;
						}
						else
						{
							temp->tail = Tr_ExpList(exp.exp, NULL);
							temp = temp->tail;
						}
					}
					size++;
				}
				if (field_matched&&tylist == NULL&&list == NULL)
				{
					return expTy(Tr_recordExp(trlist, size), actual_ty(type));
				}
				else
				{
					EM_error(a->pos, "Error: Record type and fields not matched");
				}
			}
		}
		else
		{
			EM_error(a->pos, "Error: Unbound record type <%s>", S_name(a->u.record.typ));
		}
		return expTy(Tr_noExp(), Ty_Nil());
	}
	case A_arrayExp:
	{
		Ty_ty type = S_look_ty(tenv, a->u.array.typ);
		if (type&&type->kind == Ty_array)
		{
			struct expty size = transExp(level, breakk, venv, tenv, a->u.array.size);
			struct expty init = transExp(level, breakk, venv, tenv, a->u.array.init);
			if (size.ty->kind != Ty_int)
			{
				EM_error(a->u.array.size->pos, "Error: Size of an array should be int value");
			}
			if (is_equal_ty(type->u.array, init.ty))
			{
				return expTy(Tr_arrayExp(size.exp, init.exp), actual_ty(type));
			}
			EM_error(a->pos, "Error: Array init value type not matched");
		}
		else
		{
			EM_error(a->pos, "Error: Unbound type <%s>", S_name(a->u.array.typ));
		}
		return expTy(Tr_noExp(), Ty_Void());
	}
	}
	assert(0);
	return expTy(Tr_noExp(), Ty_Void());
}

// return var in Tr_exp with its type from A_var
struct expty transVar(Tr_level level, Temp_label breakk, S_table venv, S_table tenv, A_var v)
{
	switch (v->kind)
	{
	case A_simpleVar: {
		E_enventry x = S_look(venv, v->u.simple);
		if (x&&x->kind == E_varEntry)
		{
			return expTy(Tr_simpleVar(x->u.var.access, level), actual_ty(x->u.var.ty));
		}
		else
		{
			EM_error(v->pos, "Error: Unbound value %s", S_name(v->u.simple));
		}
		return expTy(Tr_noExp(), Ty_Void());
		break;
	}
	case A_fieldVar: {
		bool is_exist = FALSE;
		int index = 0;
		struct expty field = transVar(level, breakk, venv, tenv, v->u.field.var);
		if (field.ty->kind != Ty_record)
		{
			EM_error(v->u.field.var->pos, "Error: This variable with field is not a record");
		}
		Ty_fieldList list;
		for (list = field.ty->u.record; list; list = list->tail)
		{
			if (list->head->name == v->u.field.sym)
			{
				is_exist = TRUE;
				break;
			}
			index++;
		}
		if (is_exist)
		{
			return expTy(Tr_fieldVar(field.exp, index), actual_ty(list->head->ty));
		}
		EM_error(v->pos, "Error: This record does not contain the field \"%s\"", S_name(v->u.field.sym));
		return expTy(Tr_noExp(), Ty_Void());
		break;
	}
	case A_subscriptVar: {
		struct expty ptr = transVar(level, breakk, venv, tenv, v->u.subscript.var);
		struct expty exp = transExp(level, breakk, venv, tenv, v->u.subscript.exp);
		if (ptr.ty->kind != Ty_array)
		{
			EM_error(v->pos, "Error: This variable with subscript is not an array");
			break;
		}
		if (exp.ty->kind != Ty_int)
		{
			EM_error(v->u.subscript.exp->pos, "Error: This expression was expected of type int");
			break;
		}
		return expTy(Tr_subscriptVar(ptr.exp, exp.exp), actual_ty(ptr.ty->u.array));
	}
	}
	return expTy(Tr_noExp(), Ty_Void());
}

// return Ty_ty from A_ty
Ty_ty transTy(S_table tenv, A_ty a)
{
	switch (a->kind)
	{
	case A_nameTy:
	{
		// cannot use actual type
		Ty_ty type = S_look(tenv, a->u.name);
		if (type == NULL)
		{
			EM_error(a->pos, "Error: Unbound type <%s>", S_name(a->u.name));
			type = Ty_Int();
		}
		return type;
	}
	case A_recordTy:
	{
		Ty_fieldList temp, list = makeFieldList(tenv, a->u.record);
		temp = list;
		for (temp; temp; temp = temp->tail)
		{
			// will not hit
			if (temp->head == NULL)
			{
				EM_error(a->pos, "Error: Unbound type in recordTy");
			}
		}
		return Ty_Record(list);
	}
	case A_arrayTy:
	{
		// cannot use actual type
		Ty_ty type = S_look(tenv, a->u.array);
		if (type == NULL)
		{
			EM_error(a->pos, "Error: Unbound type <%s>", S_name(a->u.array));
			type = Ty_Int();
		}
		return Ty_Array(type);
	}
	}
	assert(0);
	return Ty_Void();
}

Tr_exp transDec(Tr_level level, Temp_label breakk, S_table venv, S_table tenv, A_dec d)
{
	// assume that all member of d is complete, which is finished in parsing phase
	switch (d->kind)
	{
	case A_varDec:
	{
		Tr_access access = Tr_allocLocal(level, d->u.var.escape);
		struct expty e = transExp(level, breakk, venv, tenv, d->u.var.init);
		// checking init and def types·
		if (d->u.var.typ != NULL)
		{
			Ty_ty type = S_look(tenv, d->u.var.typ);
			if (!is_equal_ty(type, e.ty))
				//if ((e.ty->kind == Ty_nil&&type->kind != Ty_record) || type->kind != e.ty->kind)
			{
				EM_error(d->pos, "Error: The type of \"%s\" and the type of init expression mismatched", S_name(d->u.var.var));
			}
			S_enter(venv, d->u.var.var, E_VarEntry(access, type));
		}
		else
		{
			if (e.ty->kind == Ty_nil)
			{
				EM_error(d->pos, "Error: Without type constraint, init value cannot be nil");
				break;
			}
			S_enter(venv, d->u.var.var, E_VarEntry(access, e.ty));
		}
		return Tr_assignExp(Tr_simpleVar(access, level), e.exp);
	}
	// deal with recursive type defining
	case A_typeDec:
	{
		A_nametyList list = d->u.type, nest;
		Ty_ty type, temp;
		// add all types as alias
		for (list; list; list = list->tail)
		{
			type = Ty_Name(list->head->name, NULL);
			S_enter(tenv, list->head->name, type);
		}
		for (list = d->u.type; list; list = list->tail)
		{
			for (nest = list->tail; nest; nest = nest->tail)
			{
				if (list->head->name == nest->head->name)
				{
					EM_error(d->pos, "Error: There are two types <%s> with the same name in the same (consecutive) batch of mutually recursive types", S_name(type->u.name.sym));
					goto TYPE_err;
					break;
				}
			}
		}
		// update the real types
		for (list = d->u.type; list; list = list->tail)
		{
			type = S_look(tenv, list->head->name);
			type->u.name.ty = transTy(tenv, list->head->ty);
		}
		// checking cyclic
		for (list = d->u.type; list; list = list->tail)
		{
			type = S_look(tenv, list->head->name);
			temp = type;
			type = type->u.name.ty;
			while (type&&type->kind == Ty_name)
			{
				if (type->u.name.sym == temp->u.name.sym)
				{
					EM_error(d->pos, "Error: The type alias <%s> is cyclic", S_name(type->u.name.sym));
					goto TYPE_err;
					break;
				}
				type = type->u.name.ty;
			}
		}
	TYPE_err:
		return Tr_noExp();
		//return NULL;
		break;
	}
	// handle recursive func and multi func
	case A_functionDec:
	{
		A_fundecList list = d->u.function, nest;
		for (; list; list = list->tail)
		{
			A_fundec f = list->head;
			// TODO more accurate access type
			U_boolList formals = makeFormalList(f->params);
			Temp_label flabel = Temp_newlabel();
			Tr_level flevel = Tr_newLevel(level, flabel, formals);
			Ty_ty resultTy = Ty_Void();
			if (f->result)
			{
				resultTy = S_look(tenv, f->result);
			}
			if (resultTy == NULL)
			{
				EM_error(d->pos, "Error: Unbound return type <%s> of function \"%s\"", S_name(f->result), S_name(f->name));
				resultTy = Ty_Void();
			}
			if (!resultTy) resultTy = Ty_Void();
			// undefined formal types?
			Ty_tyList formalTys = makeFormalTyList(tenv, f->params);
			S_enter(venv, f->name, E_FunEntry(flevel, flabel, formalTys, resultTy));
		}
		for (list = d->u.function; list; list = list->tail)
		{
			A_fundec f = list->head;
			for (nest = list->tail; nest; nest = nest->tail)
			{
				A_fundec f_temp = nest->head;
				if (f->name == f_temp->name)
				{
					EM_error(f_temp->pos, "Error: there are two functions \"%s\" with the same in the same(consecutive) batch of mutually recursive functions", S_name(f_temp->name));
				}
			}
		}
		for (list = d->u.function; list; list = list->tail)
		{
			A_fundec f = list->head;
			E_enventry func = S_look(venv, f->name);
			Ty_ty resultTy = func->u.fun.result;
			Ty_tyList formalTys = func->u.fun.formals;
			Tr_accessList acclist = Tr_formals(func->u.fun.level);
			struct expty body;
			S_beginScope(venv);
			{
				A_fieldList l;
				Ty_tyList t;
				for (l = f->params, t = formalTys; l; l = l->tail, t = t->tail, acclist = acclist->tail)
				{
					// cannot be NULL
					if (t->head == NULL)
					{
						S_enter(venv, l->head->name, E_VarEntry(acclist->head, t->head));
						EM_error(f->pos, "Error: The type of \"%s\" is unknown in \"%s\"", S_name(l->head->name), S_name(f->name));
					}
					else
					{
						// update formals
						S_enter(venv, l->head->name, E_VarEntry(acclist->head, t->head));
					}
				}
			}
			acclist = Tr_formals(func->u.fun.level);
			body = transExp(func->u.fun.level, breakk, venv, tenv, f->body);
			Tr_procEntryExit(func->u.fun.level, body.exp, acclist);
			if (!is_equal_ty(resultTy, body.ty))
			{
				EM_error(d->pos, "Error: The return type of \"%s\" and the body type is matched", S_name(f->name));
			}
			S_endScope(venv);
			//printStmList(stdout, T_StmList(T_Label(func->u.fun.label), T_StmList(T_Move(T_Temp(F_RV()), unEx(body.exp)), NULL)));
		}
		return Tr_noExp();
		break;
	}
	}
	return Tr_noExp();
}

// the whole type checking and Tr_exp constructing phase
F_fragList SEM_transProg(A_exp exp)
{
	S_table venv = E_base_venv();
	S_table tenv = E_base_tenv();
	Tr_level level = Tr_outermost();
	struct expty temp = transExp(Tr_outermost(), NULL, venv, tenv, exp);
	Tr_procEntryExit(Tr_outermost(), temp.exp, NULL);
	//printStmList(stdout, T_StmList(T_Label(level->name), T_StmList(T_Exp(temp.exp->u.ex), NULL)));
	return Tr_getResult();
}
