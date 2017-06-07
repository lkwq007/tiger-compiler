/*
 * printtree.c - functions to print out intermediate representation (IR) trees.
 *
 */
#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "printtree.h"

 /* local function prototype */
static unsigned int pr_tree_exp(FILE *out, T_exp exp, unsigned int pid);

static char bin_oper[][12] = {
   "PLUS", "MINUS", "TIMES", "DIVIDE",
   "AND", "OR", "LSHIFT", "RSHIFT", "ARSHIFT", "XOR" };

static char rel_oper[][12] = {
  "EQ", "NE", "LT", "GT", "LE", "GE", "ULT", "ULE", "UGT", "UGE" };

static unsigned int id = 0;

static unsigned int pr_stm(FILE *out, T_stm stm, unsigned int pid)
{
	unsigned int uid = id;
	id++;
	fprintf(out, "n%x--n%x;\n", pid, uid);
	switch (stm->kind) {
	case T_SEQ:
		fprintf(out, "n%x [label=\"SEQ\"];\n", uid);
		pr_stm(out, stm->u.SEQ.left, uid);
		pr_stm(out, stm->u.SEQ.right, uid);
		break;
	case T_LABEL:
		fprintf(out, "n%x [label=\"LABEL %s\"];\n", uid, S_name(stm->u.LABEL));
		break;
	case T_JUMP:
		fprintf(out, "n%x [label=\"JUMP\"];\n", uid);
		pr_tree_exp(out, stm->u.JUMP.exp, uid);
		break;
	case T_CJUMP:
		fprintf(out, "n%x [label=\"CJUMP\"];\n", uid);
		// op
		fprintf(out, "n%x--n%x;\n", uid, id);
		fprintf(out, "n%x [label=\"%s\"];\n", id, rel_oper[stm->u.CJUMP.op]);
		id++;
		// t&f name
		fprintf(out, "n%x--n%x;\n", uid, id);
		fprintf(out, "n%x [label=\"%s\"];\n", id, S_name(stm->u.CJUMP.true));
		id++;
		fprintf(out, "n%x--n%x;\n", uid, id);
		fprintf(out, "n%x [label=\"%s\"];\n", id, S_name(stm->u.CJUMP.false));
		id++;
		// left&right
		pr_tree_exp(out, stm->u.CJUMP.left, uid);
		pr_tree_exp(out, stm->u.CJUMP.right, uid);
		break;
	case T_MOVE:
		fprintf(out, "n%x [label=\"MOVE\"];\n", uid);
		pr_tree_exp(out, stm->u.MOVE.dst, uid);
		pr_tree_exp(out, stm->u.MOVE.src, uid);
		break;
	case T_EXP:
		fprintf(out, "n%x [label=\"EXP\"];\n", uid);
		pr_tree_exp(out, stm->u.EXP, uid);
		break;
	}
	return uid;
}

static unsigned int pr_tree_exp(FILE *out, T_exp exp, unsigned int pid)
{
	unsigned int uid = id;
	id++;
	fprintf(out, "n%x--n%x;\n", pid, uid);
	switch (exp->kind) {
	case T_BINOP:
		fprintf(out, "n%x--n%x;\n", uid, id);
		fprintf(out, "n%x [label=\"%s\"];\n", id, bin_oper[exp->u.BINOP.op]);
		id++;
		fprintf(out, "n%x [label=\"BINOP\"];\n", uid);
		pr_tree_exp(out, exp->u.BINOP.left, uid);
		pr_tree_exp(out, exp->u.BINOP.right, uid);
		break;
	case T_MEM:
		fprintf(out, "n%x [label=\"MEM\"];\n", uid);
		pr_tree_exp(out, exp->u.MEM, uid);
		break;
	case T_TEMP:
		fprintf(out, "n%x [label=\"TEMP %s\"];\n", uid, Temp_look(Temp_name(), exp->u.TEMP));
		break;
	case T_ESEQ:
		fprintf(out, "n%x [label=\"ESEQ\"];\n", uid);
		pr_stm(out, exp->u.ESEQ.stm, uid);
		pr_tree_exp(out, exp->u.ESEQ.exp, uid);
		break;
	case T_NAME:
		fprintf(out, "n%x [label=\"NAME %s\"];\n", uid, S_name(exp->u.NAME));
		break;
	case T_CONST:
		fprintf(out, "n%x [label=\"CONST %d\"];\n", uid, exp->u.CONST);
		break;
	case T_CALL:
	{
		T_expList args = exp->u.CALL.args;
		fprintf(out, "n%x [label=\"CALL\"];\n", uid);
		pr_tree_exp(out, exp->u.CALL.fun, uid);
		for (; args; args = args->tail) {
			pr_tree_exp(out, args->head, uid);
		}
		break;
	}
	} /* end of switch */
	return uid;
}

void printStmList(FILE *out, T_stmList stmList)
{
	int flag = 0;
	unsigned int uid=0;
	if (id == 0)
	{
		fprintf(out, "graph \"\" {\nn0 [label=\"program\"]\n");
		id++;
	}
	if (stmList->head->kind == T_LABEL)
	{
		if (S_Symbol("main") == stmList->head->u.LABEL)
		{
			flag = 1;
		}
	}
	for (; stmList; stmList = stmList->tail) {
		uid=pr_stm(out, stmList->head, uid);
	}
	if (flag)
	{
		fprintf(out, "}\n");
	}
}
