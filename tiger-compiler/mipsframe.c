/* mipsframe.c */
#include "frame.h"
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"
Temp_map F_tempMap = NULL;
static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);

static F_access InFrame(int offset)
{
	F_access access;
	access = (F_access)checked_malloc(sizeof(struct F_access_));
	access->kind = inFrame;
	access->u.offset = offset;
	return access;
}

static F_access InReg(Temp_temp reg)
{
	F_access access;
	access = (F_access)checked_malloc(sizeof(struct F_access_));
	access->kind = inReg;
	access->u.reg = reg;
	return access;
}

static void F_map(string str, Temp_temp temp)
{
	if (!F_tempMap)
	{
		F_tempMap = Temp_name();
	}
	Temp_enter(F_tempMap, temp, str);
}

// mips $fp, $sp, $ra, $v1 => $rv (tiger can return 0 or 1 value
// Init special regs
static Temp_temp fp = NULL, ra = NULL, sp = NULL, rv=NULL;
Temp_temp F_FP(void)
{
	if (fp == NULL)
	{
		fp = Temp_newtemp();
		F_map("$fp", fp);
	}
	return fp;
}
Temp_temp F_RA(void)
{
	if (ra == NULL)
	{
		ra = Temp_newtemp();
		F_map("$ra", ra);
	}
	return ra;
}
Temp_temp F_SP(void)
{
	if (sp == NULL)
	{
		sp = Temp_newtemp();
		F_map("$sp", sp);
	}
	return sp;
}
Temp_temp F_RV(void)
{
	if (rv == NULL)
	{
		rv = Temp_newtemp();
		F_map("$v0", rv);
	}
	return rv;
}

static F_access static_link = NULL;
F_access F_staticLink()
{
	if (static_link == NULL)
	{
		static_link = InFrame(0);
	}
	return static_link;
}

// contruct f_stringfrag
F_frag F_StringFrag(Temp_label label, string str)
{
	F_frag frag = (F_frag)checked_malloc(sizeof(*frag));
	frag->kind = F_stringFrag;
	frag->u.stringg.label = label;
	frag->u.stringg.str = str;
	return frag;
}

F_frag F_ProcFrag(T_stm body, F_frame frame)
{
	F_frag frag = (F_frag)checked_malloc(sizeof(*frag));
	frag->kind = F_procFrag;
	frag->u.proc.body = body;
	frag->u.proc.frame = frame;
	return frag;
}

F_fragList F_FragList(F_frag head, F_fragList tail)
{
	F_fragList fragList = (F_fragList)checked_malloc(sizeof(*fragList));
	fragList->head = head;
	fragList->tail = tail;
	return fragList;
}

F_frame F_newFrame(Temp_label name, U_boolList formals)
{
	// TODO complete frame
	F_frame f = (F_frame)checked_malloc(sizeof(*f));
	f->name = name;
	f->formals = formals;
	f->local_count = 0;
	
	return f;
}

Temp_label F_name(F_frame f)
{
	return f->name;
}

F_accessList F_formals(F_frame f)
{
	return f->formals;
}

F_access F_allocLocal(F_frame f, bool escape)
{
	if (escape)
	{
		f->local_count++;
		return InFrame(-F_wordSize*f->local_count);
	}
	else
	{
		return InReg(Temp_newtemp());
	}
}

T_exp F_exp(F_access acc, T_exp framePtr)
{
	if (acc->kind == inFrame)
	{
		return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
	}
	else
	{
		return T_Temp(acc->u.reg);
	}
}

T_exp F_externalCall(string str, T_expList args)
{
	return T_Call(T_Name(Temp_namedlabel(str)), args);
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm)
{
	return stm;
}

static Temp_tempList returnSink = NULL;
AS_instrList F_procEntryExit2(AS_instrList body)
{
	if (!returnSink)
	{	
		returnSink = Temp_TempList(F_RA(), F_callee_saves());
	}
	return AS_splice(body, AS_InstrList(AS_Oper("", NULL, returnSink, NULL), NULL));
}

AS_proc F_procEntryExit3(F_frame frame, AS_instrList body)
{
	return AS_Proc("prolog", body, "epilog");
}