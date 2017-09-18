#include "worklist.h"
// list for move instructions
moveInstr MoveInstr(AS_instr move, M_INSTR kind)
{
	moveInstr p = checked_malloc(sizeof *p);
	p->move = move;
	p->kind = kind;
	return p;
}

moveList MoveList(moveInstr head, moveList prev ,moveList tail)
{
	moveList p = checked_malloc(sizeof *p);
	p->head = head;
	p->prev = prev;
	p->tail = tail;
	return p;
}

tempInfo TempInfo(Temp_temp temp)
{
	tempInfo p = checked_malloc(sizeof *p);
	p->temp = temp;
	p->adjList = NULL;
	p->degree = 0;
	p->moveList = NULL;
	p->color = NULL;
	p->alias = NULL;
	return p;
};

typedef struct TAB_table_ *T_table;

static T_table T_empty(void)
{
	return TAB_empty();
}

void T_enter(T_table t, Temp_temp temp, void *value)
{
	TAB_enter(t, temp, value);
}

void *T_look(T_table t, Temp_temp temp)
{
	return TAB_look(t, temp);
}