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
static void pr_tree_exp(FILE *out, T_exp exp, int d);

static void indent(FILE *out, int d) {
	int i;
	for (i = 0; i <= d; i++) fprintf(out, " ");
}

static char bin_oper[][12] = {
   "PLUS", "MINUS", "TIMES", "DIVIDE",
   "AND", "OR", "LSHIFT", "RSHIFT", "ARSHIFT", "XOR" };

static char rel_oper[][12] = {
  "EQ", "NE", "LT", "GT", "LE", "GE", "ULT", "ULE", "UGT", "UGE" };

static void pr_stm(FILE *out, T_stm stm, int d)
{
	switch (stm->kind) {
	case T_SEQ:
		fprintf(out, "n%x--n%x;\n", stm, stm->u.SEQ.left);
		fprintf(out, "n%x--n%x;\n", stm, stm->u.SEQ.right);
		fprintf(out, "n%x [label=\"SEQ\"];\n",stm);
		pr_stm(out, stm->u.SEQ.left, d + 1);
		pr_stm(out, stm->u.SEQ.right, d + 1);
		break;
	case T_LABEL:
		fprintf(out, "n%x [label=\"LABEL %s\"];\n",stm, S_name(stm->u.LABEL));
		break;
	case T_JUMP:
		fprintf(out, "n%x--n%x;\n", stm, stm->u.JUMP.exp);
		fprintf(out, "n%x [label=\"JUMP\"];\n", stm);
		pr_tree_exp(out, stm->u.JUMP.exp, d + 1);
		break;
	case T_CJUMP:
		fprintf(out, "n%x [label=\"CJUMP\"];\n", stm);
		// op
		fprintf(out, "n%x--n%x;\n", stm, &(stm->u.CJUMP.op));
		fprintf(out, "n%x [label=\"%s\"];\n", &(stm->u.CJUMP.op), rel_oper[stm->u.CJUMP.op]);
		// t&f name
		fprintf(out, "n%x--n%x;\n", stm, &(stm->u.CJUMP.true));
		fprintf(out, "n%x [label=\"%s\"];\n", &(stm->u.CJUMP.true), S_name(stm->u.CJUMP.true));
		fprintf(out, "n%x--n%x;\n", stm, &(stm->u.CJUMP.false));
		fprintf(out, "n%x [label=\"%s\"];\n", &(stm->u.CJUMP.false), S_name(stm->u.CJUMP.false));
		// left&right
		fprintf(out, "n%x--n%x;\n", stm, stm->u.CJUMP.left);
		fprintf(out, "n%x--n%x;\n", stm, stm->u.CJUMP.right);
		pr_tree_exp(out, stm->u.CJUMP.left, d + 1);
		pr_tree_exp(out, stm->u.CJUMP.right, d + 1);
		break;
	case T_MOVE:
		fprintf(out, "n%x--n%x;\n", stm, stm->u.MOVE.dst);
		fprintf(out, "n%x--n%x;\n", stm, stm->u.MOVE.src);
		fprintf(out, "n%x [label=\"MOVE\"];\n", stm);
		pr_tree_exp(out, stm->u.MOVE.dst, d + 1);
		pr_tree_exp(out, stm->u.MOVE.src, d + 1);
		break;
	case T_EXP:
		fprintf(out, "n%x--n%x;\n", stm, stm->u.EXP);
		fprintf(out, "n%x [label=\"EXP\"];\n", stm);
		pr_tree_exp(out, stm->u.EXP, d + 1);
		break;
	}
}

static void pr_tree_exp(FILE *out, T_exp exp, int d)
{
	switch (exp->kind) {
	case T_BINOP:
		fprintf(out, "n%x--n%x;\n", exp, &(exp->u.BINOP.op));
		fprintf(out, "n%x [label=\"%s\"];\n", &(exp->u.BINOP.op), bin_oper[exp->u.BINOP.op]);
		fprintf(out, "n%x--n%x;\n", exp, exp->u.BINOP.left);
		fprintf(out, "n%x--n%x;\n", exp, exp->u.BINOP.right);
		fprintf(out, "n%x [label=\"BINOP\"];\n", exp);
		pr_tree_exp(out, exp->u.BINOP.left, d + 1);
		pr_tree_exp(out, exp->u.BINOP.right, d + 1);
		break;
	case T_MEM:
		fprintf(out, "n%x--n%x;\n", exp, exp->u.MEM);
		fprintf(out, "n%x [label=\"MEM\"];\n", exp);
		pr_tree_exp(out, exp->u.MEM, d + 1);
		break;
	case T_TEMP:
		fprintf(out, "n%x [label=\"TEMP %s\"];\n", exp,Temp_look(Temp_name(), exp->u.TEMP));
		break;
	case T_ESEQ:
		fprintf(out, "n%x--n%x;\n", exp, exp->u.ESEQ.stm);
		fprintf(out, "n%x--n%x;\n", exp, exp->u.ESEQ.exp);
		fprintf(out, "n%x [label=\"ESEQ\"];\n", exp);
		pr_stm(out, exp->u.ESEQ.stm, d + 1);
		pr_tree_exp(out, exp->u.ESEQ.exp, d + 1);
		break;
	case T_NAME:
		fprintf(out, "n%x [label=\"NAME %s\"];\n", exp, S_name(exp->u.NAME));
		break;
	case T_CONST:
		fprintf(out, "n%x [label=\"CONST %d\"];\n", exp, exp->u.CONST);
		break;
	case T_CALL:
	{
		T_expList args = exp->u.CALL.args;
	fprintf(out, "n%x [label=\"CALL\"];\n", exp);
	fprintf(out, "n%x--n%x;\n", exp, exp->u.CALL.fun);
	pr_tree_exp(out, exp->u.CALL.fun, d + 1);
	for (; args; args = args->tail) {
		fprintf(out, "n%x--n%x;\n", exp, args->head);
		pr_tree_exp(out, args->head, d + 2);
	}
	break;
	}
	} /* end of switch */
}

void printStmList(FILE *out, T_stmList stmList)
{
	if (stmList->head->kind == T_LABEL)
	{
		fprintf(out, "graph \"%s\" {\n", S_name(stmList->head->u.LABEL));
	}
	else
	{
		fprintf(out, "graph \"%x\" {\n", stmList);
	}
	for (; stmList; stmList = stmList->tail) {
		pr_stm(out, stmList->head, 0);
	}
	fprintf(out, "}\n");
}
