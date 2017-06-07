#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include "codegen.h"
#include "assem.h"
#include "tree.h"
#include "frame.h"
#include "temp.h"
#include <assert.h>

static void emit(AS_instr inst);
static void munchStm(T_stm stm);
static Temp_temp munchExp(T_exp expr);
static Temp_tempList munchArgs(unsigned int n, T_expList eList);

static char *arg_reg[] = { "$a0", "$a1", "$a2", "$a3" };
static Temp_tempList munchArgs(unsigned int n, T_expList eList)
{
	if (!eList)
	{
		return NULL;
	}
	Temp_tempList tlist = munchArgs(n + 1, eList->tail);
	Temp_temp r = munchExp(eList->head);
	emit(AS_Oper("addi $sp, $sp, -4\nsw `s0, 0($sp) # formal\n", NULL, Temp_TempList(r, NULL), NULL));
	return Temp_TempList(r, tlist);
}

int remunchArgs(T_expList eList)
{
	if (!eList)
	{
		return 0;
	}
	return 1 + remunchArgs(eList->tail);
}

// emit the AS instrList
static AS_instrList iList = NULL, last = NULL;
static void emit(AS_instr inst)
{
	if (last != NULL)
	{
		last = last->tail = AS_InstrList(inst, NULL);
	}
	else
	{
		last = iList = AS_InstrList(inst, NULL);
	}
}

static Temp_temp munchExp(T_exp e)
{
	string buf = (string)checked_malloc(128);
	switch (e->kind)
	{
	case T_MEM:
	{
		T_exp mem = e->u.MEM;
		if (mem->kind == T_BINOP)
		{
			Temp_temp r = Temp_newtemp();
			if (mem->u.BINOP.left->kind == T_CONST)
			{
				// MEM(BINOP(PLUS, CONST(i), e1))
				T_exp e1 = mem->u.BINOP.right;
				int offset = mem->u.BINOP.left->u.CONST;
				sprintf(buf, "lw `d0 %d(`s0)\n", offset);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else if (mem->u.BINOP.right->kind == T_CONST)
			{
				// MEM(BINOP(PLUS, e1, CONST(i)))
				T_exp e1 = mem->u.BINOP.left;
				int offset = mem->u.BINOP.right->u.CONST;
				sprintf(buf, "lw `d0 %d(`s0)\n", offset);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				T_exp e1 = mem->u.BINOP.left;
				T_exp e2 = mem->u.BINOP.right;
				sprintf(buf, "add $t0, `s0, `s1\nlw `d0 0($t0)\n");
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2),NULL)), NULL));
			}
		}
		if (mem->kind == T_CONST)
		{
			// MEM(CONST(i))
			Temp_temp r = Temp_newtemp();
			int offset = mem->u.CONST;
			sprintf(buf, "lw `d0 %d($r0)\n", offset);
			emit(AS_Oper(buf, Temp_TempList(r, NULL), NULL, NULL));
			return r;
		}
		else
		{
			// MEM(e1)
			Temp_temp r = Temp_newtemp();
			T_exp e1 = mem->u.MEM;
			if (e1 == NULL)
			{
				return r;
			}
			sprintf(buf, "lw `d0, 0(`s0)\n");
			emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
			return r;
		}
	}
	case T_BINOP:
	{
		// TODO most op
		Temp_temp r = Temp_newtemp();
		T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
		switch (e->u.BINOP.op)
		{
		case T_plus:
		{
			if (e1->kind == T_CONST)
			{
				// BINOP(op, CONST(i), e2)
				sprintf(buf, "addi `d0, `s0, %d\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST)
			{
				// BINOP(op, e1, CONST(i))
				sprintf(buf, "addi `d0, `s0, %d\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				// BINOP(op, e1, e2)
				emit(AS_Oper("add `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
		}
		case T_minus:
		{
			if (e1->kind == T_CONST)
			{
				// BINOP(op, CONST(i), e2)
				sprintf(buf, "addi `d0, `s0, -(%d)\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST)
			{
				// BINOP(op, e1, CONST(i))
				sprintf(buf, "addi `d0, `s0, -(%d)\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				// BINOP(op, e1, e2)
				emit(AS_Oper("sub `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
		}
		case T_mul:
		{
			if (e1->kind == T_CONST)
			{
				// BINOP(op, CONST(i), e2)
				sprintf(buf, "li $t0, %d\nmult `s0, $t0\nmflo `d0\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST)
			{
				// BINOP(op, e1, CONST(i))
				sprintf(buf, "li $t0, %d\nmult `s0, $t0\nmflo `d0\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				// BINOP(op, e1, e2)
				emit(AS_Oper("mult `s0, `s1\nmflo `d0\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
		}
		case T_div:
		{
			if (e1->kind == T_CONST)
			{
				// BINOP(op, CONST(i), e2)
				sprintf(buf, "li $t0, %d\ndiv $t0, `s0\nmflo `d0\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST)
			{
				// BINOP(op, e1, CONST(i))
				sprintf(buf, "li $t0, %d\ndiv `s0, $t0\nmflo `d0\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				// BINOP(op, e1, e2)
				emit(AS_Oper("div `s0, `s1\nmflo `d0\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
			return r;
		}
		case T_and:
		{
			if (e1->kind == T_CONST)
			{
				sprintf(buf, "andi `d0, `s0, %d\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST) {
				sprintf(buf, "andi `d0, `s0, %d\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				emit(AS_Oper("and `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
		}
		case T_or:
		{
			if (e1->kind == T_CONST)
			{
				sprintf(buf, "ori `d0, `s0, %d\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST)
			{
				sprintf(buf, "ori `d0, `s0, %d\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else
			{
				emit(AS_Oper("or `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
		}
		case T_lshift:
		{
			emit(AS_Oper("sll `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			return r;
		}
		case T_rshift:
		{
			emit(AS_Oper("srl `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			return r;
		}
		case T_arshift:
		{
			emit(AS_Oper("sra `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			return r;
		}
		case T_xor:
		{
			if (e1->kind == T_CONST)
			{
				sprintf(buf, "xori `d0, `s0, %d\n", e1->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e2), NULL), NULL));
				return r;
			}
			else if (e2->kind == T_CONST)
			{
				sprintf(buf, "xori `d0, `s0, %d\n", e2->u.CONST);
				emit(AS_Oper(buf, Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), NULL), NULL));
				return r;
			}
			else {
				emit(AS_Oper("xor `d0, `s0, `s1\n", Temp_TempList(r, NULL), Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
				return r;
			}
		}
		default:
			assert(0);
		}
	}
	case T_TEMP:
	{
		// TEMP(t)
		if (e->u.TEMP)
		{
			return e->u.TEMP;
		}
		return Temp_newtemp();
	}
	case T_ESEQ:
	{
		// ESEQ(s, e)
		munchStm(e->u.ESEQ.stm);
		return munchExp(e->u.ESEQ.exp);
	}
	case T_NAME:
	{
		// NAME(n)
		Temp_temp r = Temp_newtemp();
		Temp_enter(F_tempMap, r, Temp_labelstring(e->u.NAME));
		return r;
	}
	case T_CONST:
	{
		Temp_temp r = Temp_newtemp();
		sprintf(buf, "li `d0, %d\n", e->u.CONST);
		emit(AS_Oper(buf, Temp_TempList(r, NULL), NULL, NULL));
		return r;
	}
	case T_CALL:
	{
		// TODO: fix call
		emit(AS_Oper("addi $sp, $sp, -4\nsw $ra, 0($sp)\n", NULL, NULL, NULL));
		Temp_temp r=munchExp(e->u.CALL.fun);
		Temp_tempList list=munchArgs(0,e->u.CALL.args);
		//emit(AS_Oper("jal `s0\n", calldefs, Temp_TempList(r, list), NULL));
		emit(AS_Oper("jal `s0\n", NULL, Temp_TempList(r, list), NULL));
		int num = remunchArgs(e->u.CALL.args);
		sprintf(buf, "addi $sp, $sp, %d\n", num*F_wordSize);
		emit(AS_Oper(buf, NULL, NULL, NULL));
		emit(AS_Oper("ld $ra, 0($sp)\naddi $sp, $sp, 4\n", NULL, NULL, NULL));
		return F_RV();
	}
	default:
	{
		assert(0);
	}
	}
	return Temp_newtemp();
}

static void munchStm(T_stm s)
{
	string buf = (string)checked_malloc(128);
	switch (s->kind) {
	case T_MOVE:
	{
		T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
		if (dst->kind == T_MEM)
		{
			if (dst->u.MEM->kind == T_BINOP &&dst->u.MEM->u.BINOP.op == T_plus &&dst->u.MEM->u.BINOP.right->kind == T_CONST)
			{
				// MOVE(MEM(BINOP(PLUS, e1, CONST(n))), e2)
				T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
				int num = dst->u.MEM->u.BINOP.right->u.CONST;
				sprintf(buf, "sw `s1, %d(`s0)\n", num);
				emit(AS_Oper(buf, NULL, Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			}
			else if (dst->u.MEM->kind == T_BINOP && dst->u.MEM->u.BINOP.op == T_plus && dst->u.MEM->u.BINOP.left->kind == T_CONST)
			{
				// MOVE(MEM(BINOP(PLUS, CONST(n), e1)), e2)
				T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
				int num = dst->u.MEM->u.BINOP.left->u.CONST;
				sprintf(buf, "sw `s1, %d(`s0)\n", num);
				emit(AS_Oper(buf, NULL, Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			}
			else if (dst->u.MEM->kind == T_CONST)
			{
				// MOVE(MEM(CONST(n)), e1)
				T_exp e1=src;
				int num = dst->u.MEM->u.CONST;
				sprintf(buf, "sw `s0, %d", num);
				emit(AS_Oper(buf, NULL, Temp_TempList(munchExp(e1), NULL), NULL));
			}
			else if (src->kind == T_MEM)
			{
				// TODO proper inst
				// MOVE(MEM(e1), MEM(e2))
				T_exp e1 = dst->u.MEM, e2 = src->u.MEM;
				sprintf(buf, "lw $t0, 0(`s1)\nsw $t0, 0(`s0)\n");
				emit(AS_Oper(buf, NULL, Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			}
			else
			{
				// MOVE(MEM(e1), e2)
				T_exp e1 = dst->u.MEM, e2 = src;
				sprintf(buf, "sw `s1, 0(`s0)\n");
				emit(AS_Oper(buf, NULL, Temp_TempList(munchExp(e1), Temp_TempList(munchExp(e2), NULL)), NULL));
			}
		}
		else if (dst->kind == T_TEMP)
		{
			// MOVE(TEMP(e1), e2)
			sprintf(buf, "move `d0, `s0\n");
			emit(AS_Move(buf, Temp_TempList(munchExp(dst), NULL), Temp_TempList(munchExp(src), NULL)));
			// TODO change to AS_move
			// now not consider reg alloc
			//emit(AS_Oper(buf, Temp_TempList(munchExp(dst), NULL), Temp_TempList(munchExp(src), NULL), NULL));
		}
		else
		{
			assert(0);
		}
		break;
	}
	case T_SEQ:
	{
		// SEQ(stm, stm)
		munchStm(s->u.SEQ.left);
		munchStm(s->u.SEQ.right);
		break;
	}
	case T_LABEL:
	{
		// LABETemp_TempList()
		sprintf(buf, "%s:\n", Temp_labelstring(s->u.LABEL));
		emit(AS_Label(buf,s->u.LABEL));
		break;
	}
	case T_JUMP:
	{
		// TODO jump!
		T_exp e = s->u.JUMP.exp;
		Temp_labelList jumps = s->u.JUMP.jumps;
		sprintf(buf, "j `j0\n");
		emit(AS_Oper(buf, NULL, NULL, AS_Targets(jumps)));
		break;
	}
	case T_CJUMP:
	{
		T_exp left = s->u.CJUMP.left;
		T_exp right = s->u.CJUMP.right;
		switch (s->u.CJUMP.op)
		{
		case T_eq:
			sprintf(buf, "beq `s0, `s1, `j0\n");
			break;
		case T_ne:
			sprintf(buf, "bne `s0, `s1, `j0\n");
			break;
		case T_lt:
			sprintf(buf, "blt `s0, `s1, `j0\n");
			break;
		case T_le:
			sprintf(buf, "ble `s0, `s1, `j0\n");
			break;
		case T_gt:
			sprintf(buf, "bgt `s0, `s1, `j0\n");
			break;
		case T_ge:
			sprintf(buf, "bge `s0, `s1, `j0\n");
			break;
		case T_ult:
			sprintf(buf, "blt `s0, `s1, `j0\n");
			break;
		case T_ule:
			sprintf(buf, "ble `s0, `s1, `j0\n");
			break;
		case T_ugt:
			sprintf(buf, "bgt `s0, `s1, `j0\n");
			break;
		case T_uge:
			sprintf(buf, "bge `s0, `s1, `j0\n");
			break;
		}
		emit(AS_Oper(buf, NULL, Temp_TempList(munchExp(left), Temp_TempList(munchExp(right), NULL)), AS_Targets(Temp_LabelList(s->u.CJUMP.true, NULL))));
		break;
	}
	case T_EXP:
	{
		munchExp(s->u.EXP);
		break;
	}
	default:
	{
		assert(0);
	}
	}
	return;
}

AS_instrList F_codegen(F_frame f, T_stmList stmList)
{
	AS_instrList list;
	T_stmList sl;
	for (sl = stmList; sl; sl = sl->tail)
	{
		munchStm(sl->head);
	}
	list = iList;
	iList = last = NULL;
	return list;
}