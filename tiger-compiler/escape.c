/*
 * the escape module will find those escaped variables
 */
#include "escape.h"
#include "symbol.h"
#include "absyn.h"
#include "env.h"
#include "util.h"
/* escape.c */

// escapeEntry binding in escape table
typedef struct escapeEntry_ *escapeEntry;

struct escapeEntry_
{
	int depth;
	bool *escape;
};

static escapeEntry EscapeEntry(int depth, bool *escpae)
{
	escapeEntry entry = (escapeEntry)checked_malloc(sizeof(*entry));
	entry->depth = depth;
	entry->escape = escpae;
	return entry;
}

static void traverseExp(S_table env, int depth, A_exp e);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);

// traverseExp to get escape info
static void traverseExp(S_table env, int depth, A_exp e)
{
	switch (e->kind)
	{
	case A_varExp:
	{
		traverseVar(env, depth, e->u.var);
		break;
	}
	case A_nilExp:
	case A_intExp:
	case A_stringExp:
	{
		break;
	}
	case A_callExp:
	{
		A_expList list;
		for (list = e->u.call.args; list; list = list->tail)
		{
			traverseExp(env, depth, list->head);
		}
		break;
	}
	case A_opExp:
	{
		traverseExp(env, depth, e->u.op.left);
		traverseExp(env, depth, e->u.op.right);
		break;
	}
	case A_seqExp:
	{
		A_expList list = e->u.seq;
		for (; list; list = list->tail)
		{
			traverseExp(env, depth, list->head);
		}
		break;
	}
	case A_assignExp:
	{
		traverseVar(env, depth, e->u.assign.var);
		traverseExp(env, depth, e->u.assign.exp);
		break;
	}
	case A_ifExp:
	{
		traverseExp(env, depth, e->u.iff.test);
		traverseExp(env, depth, e->u.iff.then);
		if (e->u.iff.elsee)
		{
			traverseExp(env, depth, e->u.iff.elsee);
		}
		break;
	}
	case A_whileExp:
	{
		traverseExp(env, depth, e->u.whilee.test);
		traverseExp(env, depth, e->u.whilee.body);
		break;
	}
	case A_forExp:
	{
		traverseExp(env, depth, e->u.forr.lo);
		traverseExp(env, depth, e->u.forr.hi);
		S_beginScope(env);
		S_enter(env, e->u.forr.var, EscapeEntry(depth, &(e->u.forr.escape)));
		traverseExp(env, depth, e->u.forr.body);
		S_endScope(env);
		break;
	}
	case A_breakExp:
	{
		break;
	}
	case A_letExp:
	{
		A_decList d;
		S_beginScope(env);
		for (d = e->u.let.decs; d; d = d->tail)
		{
			traverseDec(env, depth, d->head);
		}
		traverseExp(env, depth, e->u.let.body);
		S_endScope(env);
		break;
	}
	case A_recordExp:
	{
		A_efieldList list = e->u.record.fields;
		for (list; list; list = list->tail)
		{
			traverseExp(env, depth, list->head->exp);
		}
		break;
	}
	case A_arrayExp:
	{
		traverseExp(env, depth, e->u.array.size);
		traverseExp(env, depth, e->u.array.init);
		break;
	}
	}
	return;
}

// traverseDec to get escpae address & info, updating depth
static void traverseDec(S_table env, int depth, A_dec d)
{
	switch (d->kind)
	{
	case A_varDec:
	{
		traverseExp(env, depth, d->u.var.init);
		d->u.var.escape = FALSE;
		S_enter(env, d->u.var.var, EscapeEntry(depth, &(d->u.var.escape)));
		break;
	}
	case A_typeDec:
	{
		break;
	}
	case A_functionDec:
	{
		A_fundecList list;
		for (list = d->u.function; list; list = list->tail)
		{
			A_fundec f = d->u.function->head;
			S_beginScope(env);
			{
				A_fieldList l;
				for (l = f->params; l; l = l->tail)
				{
					l->head->escape = FALSE;
					S_enter(env, l->head->name, EscapeEntry(depth + 1, &(l->head->escape)));
				}
			}
			traverseExp(env, depth + 1, f->body);
			S_endScope(env);
		}
		break;
	}
	}
	return;
}

// judging whether a var is escaped
static void traverseVar(S_table env, int depth, A_var v)
{
	switch (v->kind)
	{
	case A_simpleVar: {
		escapeEntry x = S_look(env, v->u.simple);
		if (x&&x->depth < depth)
		{
			*(x->escape) = TRUE;
		}
		break;
	}
	case A_fieldVar: {
		traverseVar(env, depth, v->u.field.var);
		break;
	}
	case A_subscriptVar: {
		traverseVar(env, depth, v->u.subscript.var);
		traverseExp(env, depth, v->u.subscript.exp);
		break;
	}
	}
	return;
}

void Esc_findEscape(A_exp exp)
{
	S_table env = E_base_venv();
	traverseExp(env, 0, exp);
	return;
}
