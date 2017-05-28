/* translate.c */
#include "translate.h"
#include "frame.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"

static Tr_level outer = NULL;
F_fragList stringList = NULL;
F_fragList fragList = NULL;

T_exp Tr_expListConversion(Tr_expList stmts);
Tr_accessList makeFormalTrAccessList(Tr_level level, F_frame frame);
static void doPatch(patchList pList, Temp_label label);
Tr_access Tr_Access(Tr_level level, F_access access) {
	Tr_access list = checked_malloc(sizeof(*list));
	list->level = level;
	list->access = access;
	return list;
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
	Tr_accessList list = checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

patchList PatchList(Temp_label *head, patchList tail) {
	patchList list = checked_malloc(sizeof(*list));
	(list->head) = head;
	list->tail = tail;
	return list;
}

static Tr_exp Tr_Ex(T_exp ex) {
	Tr_exp res = checked_malloc(sizeof(*res));
	res->kind = Tr_ex;
	res->u.ex = ex;
	return res;
}
static Tr_exp Tr_Nx(T_stm nx) {
	Tr_exp res = checked_malloc(sizeof(*res));
	res->kind = Tr_nx;
	res->u.nx = nx;
	return res;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm) {
	Tr_exp res = checked_malloc(sizeof(*res));
	res->kind = Tr_cx;
	Cx cx = (Cx)checked_malloc(sizeof(*cx));
	cx->trues = trues;
	cx->falses = falses;
	cx->stm = stm;
	res->u.cx = cx;
	return res;
}


static T_exp unEx(Tr_exp e)
{
	switch (e->kind) {
	case Tr_ex:
		return e->u.ex;
	case Tr_nx:
		return T_Eseq(e->u.nx, T_Const(0));
	case Tr_cx: {
		Temp_temp r = Temp_newtemp();
		Temp_label t = Temp_newlabel(), f = Temp_newlabel();
		doPatch(e->u.cx->trues, t);
		doPatch(e->u.cx->falses, f);
		return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
			T_Eseq(e->u.cx->stm,
				T_Eseq(T_Label(f),
					T_Eseq(T_Move(T_Temp(r), T_Const(0)),
						T_Eseq(T_Label(t), T_Temp(r))))));
	}
	default: {
		assert(0);
	}
	}
	return NULL;
}

static T_stm unNx(Tr_exp e)
{
	switch (e->kind) {
	case Tr_ex:
		return T_Exp(e->u.ex);
	case Tr_nx:
		return e->u.nx;
	case Tr_cx: {
		Temp_temp r = Temp_newtemp();
		Temp_label t = Temp_newlabel(), f = Temp_newlabel();
		doPatch(e->u.cx->trues, t);
		doPatch(e->u.cx->falses, f);
		return T_Exp(T_Eseq(T_Move(T_Temp(r), T_Const(1)),
			T_Eseq(e->u.cx->stm,
				T_Eseq(T_Label(f),
					T_Eseq(T_Move(T_Temp(r), T_Const(0)),
						T_Eseq(T_Label(t), T_Temp(r)))))));
	}
	default: {
		assert(0);
	}
	}
	return NULL;
}

static Cx unCx(Tr_exp e)
{
	switch (e->kind) {
	case Tr_ex: {
		Cx cx;
		/* If comparison yields true then the expression was false (compares equal
		 * to zero) so we jump to false label. */
		cx = (Cx)checked_malloc(sizeof(*cx));
		cx->stm = T_Cjump(T_eq, e->u.ex, T_Const(0), NULL, NULL);
		cx->trues = PatchList(&(cx->stm->u.CJUMP.false), NULL);
		cx->falses = PatchList(&(cx->stm->u.CJUMP.true), NULL);
		return cx;
	}
	case Tr_nx: {
		assert(0); // Should never occur
	}
	case Tr_cx: {
		return e->u.cx;
	}
	default: {
		assert(0);
	}
	}
	return NULL;
}

static void doPatch(patchList pList, Temp_label label)
{
	for (; pList; pList = pList->tail)
		*(pList->head) = label;
}

static patchList joinPatch(patchList fList, patchList sList)
{
	if (!fList) return sList;
	for (; fList->tail; fList = fList->tail);
	fList->tail = sList;
	return fList;
}

Tr_level Tr_outermost(void) {
	if (!outer)
		outer = Tr_newLevel(NULL, Temp_newlabel(), NULL);
	return outer;
}
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
	Tr_level res = checked_malloc(sizeof(*res));
	res->name = name;
	res->parent = parent;
	res->frame = F_newFrame(name, U_BoolList(TRUE, formals));
	res->formals = makeFormalTrAccessList(res, res->frame);
	return res;
}

Tr_accessList makeFormalTrAccessList(Tr_level level, F_frame frame) {
	Tr_accessList head = NULL, tail = NULL;
	// F_accessList f_list = frame->formals->tail;
	if (!frame->formals)
	{
		return NULL;
	}
	for (F_accessList f_list = frame->formals->tail; f_list; f_list = f_list->tail)
		head = Tr_AccessList(Tr_Access(level, f_list->head), head);
	return head;
}

Tr_accessList Tr_formals(Tr_level level) {
	return level->formals;
}
Tr_access Tr_allocLocal(Tr_level level, bool escape) {
	Tr_access local = checked_malloc(sizeof(*local));
	local->level = level;
	// local var stores in register by default
	local->access = F_allocLocal(level->frame, escape);
	return local;
}

Tr_exp Tr_numExp(int num) {
	return Tr_Ex(T_Const(num));
}
Tr_exp Tr_stringExp(string str) {
	Temp_label newLabel = Temp_newlabel();
	//Tr_exp expr = Tr_Nx(T_Name(newLabel));
	T_exp expr = T_Name(newLabel);
	stringList = F_FragList(F_StringFrag(newLabel, str), stringList);
	return Tr_Ex(expr);
}

T_expList translateToTreeExpList(Tr_expList argList) {
	T_expList res = NULL;
	T_expList tailList = NULL;
    if(!argList) return NULL;
	Tr_exp iter = argList->head;
	for (; iter ; iter = argList->tail) {
		if (res) {
			tailList->tail = T_ExpList(unEx(iter), NULL);
			tailList = tailList->tail;
		} else {
			res = T_ExpList(unEx(iter), NULL);
			tailList = res;
		}
	}
	return res;
}

Tr_exp Tr_callExp(Tr_level level, Temp_label label, Tr_expList argList) {
	T_expList tempList = translateToTreeExpList(argList);
	return Tr_Ex(T_Call(T_Name(label), tempList));
}
Tr_exp Tr_opExp(int op, Tr_exp left, Tr_exp right) {
	T_binOp oper;
	switch (op) {
	case A_plusOp:
		oper = T_plus;
		break;
	case A_minusOp:
		oper = T_minus;
		break;
	case A_timesOp:
		oper = T_mul;
		break;
	case A_divideOp:
		oper = T_div;
		break;
	default:
		assert(0);
	}
	return Tr_Ex(T_Binop(op, unEx(left), unEx(right)));
}
Tr_exp Tr_relExp(int op, Tr_exp left, Tr_exp right) {
	T_relOp oper;
	switch (op) {
	case A_eqOp:
		oper = T_eq;
		break;
	case A_neqOp:
		oper = T_ne;
		break;
	case A_ltOp:
		oper = T_lt;
		break;
	case A_leOp:
		oper = T_le;
		break;
	case A_gtOp:
		oper = T_gt;
		break;
	case A_geOp:
		oper = T_ge;
		break;
	default:
		assert(0);
	}
	T_stm cond = T_Cjump(oper, unEx(left), unEx(right), NULL, NULL);
	patchList trues = PatchList(&cond->u.CJUMP.true, NULL);
	patchList falses = PatchList(&cond->u.CJUMP.false, NULL);
	return Tr_Cx(trues, falses, cond);
}
Tr_exp Tr_stringRelExp(int op, Tr_exp left, Tr_exp right) {
	return Tr_Ex(F_externalCall("stringCompare", T_ExpList(unEx(left), T_ExpList(unEx(right), NULL))));
}
Tr_exp Tr_recordExp(Tr_expList fields, int size) {
	Temp_temp r = Temp_newtemp();
	T_stm alloc = T_Move(T_Temp(r),
		F_externalCall(String("initRecord"), T_ExpList(T_Const(size * F_wordSize), NULL)));
	T_stm seq = NULL;
	for (int i = size - 1; fields; fields = fields->tail, i--)
		seq = T_Seq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(r), T_Const(i * F_wordSize))),
			unEx(fields->head)), seq);
	return Tr_Ex(T_Eseq(T_Seq(alloc, seq), T_Temp(r)));
}
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init) {
	return Tr_Ex(F_externalCall(String("initArray"),
		T_ExpList(unEx(size), T_ExpList(unEx(init), NULL))));
}
Tr_exp Tr_seqExp(Tr_expList stmts) {
	return Tr_Ex(Tr_expListConversion(stmts));
}
// auxiliary function for recursively define Tr_seqExp
T_exp Tr_expListConversion(Tr_expList stmts) {
	if (!stmts)
		return T_Const(0); //whether return NULL affects tree conversion? suppose not
	else
		return T_Eseq(T_Exp(unEx(stmts->head)), Tr_expListConversion(stmts->tail));
}
// ifExp is hard to write
Tr_exp Tr_ifExp(Tr_exp cond, Tr_exp then_, Tr_exp else_) {
	Temp_label t = Temp_newlabel(), f = Temp_newlabel(), join = Temp_newlabel();
	Cx condition = unCx(cond);
	Temp_temp r = Temp_newtemp();
	Tr_exp result = NULL;
	T_stm joinJump = T_Jump(T_Name(join), Temp_LabelList(join, NULL));
	doPatch(condition->trues, t);
	doPatch(condition->falses, f);
	if (else_->kind == Tr_ex){
		// return Tr_noExp();
	//todo fix!
        T_exp then_ex = unEx(then_);
        T_exp else_ex = unEx(else_);
		return Tr_Ex(T_Eseq(condition->stm,T_Eseq(T_Label(t), T_Eseq(T_Move(T_Mem(T_Temp(r)), then_ex),T_Eseq(joinJump, T_Eseq(T_Label(f), T_Eseq(T_Move(T_Mem(T_Temp(r)), else_ex), T_Eseq(joinJump, T_Eseq(T_Label(join), T_Temp(r))))))))));
        // return Tr_Ex(T_Eseq(condition->stm, T_Eseq(T_Label(t), 
        //         T_Eseq(T_Move(T_Temp(r), then_ex),
        //         T_Eseq(joinJump, T_Eseq(T_Label(f),
        //                 T_Eseq(T_Move(T_Temp(r), else_ex), 
        //                     T_Eseq(joinJump, 
        //                     T_Eseq(T_Label(join), T_Temp(r))))))))));
        }
	else {
        T_stm then_stm;
        if (then_->kind == Tr_ex) then_stm = T_Exp(then_->u.ex);
        else then_stm = (then_->kind == Tr_nx) ? then_->u.nx : then_->u.cx->stm;
        T_stm else_stm = (else_->kind == Tr_nx) ? else_->u.nx : else_->u.cx->stm;
		// T_stm else_stm = unNx(else_), then_stm = unNx(then_);
		return Tr_Nx(T_Seq(condition->stm, T_Seq(T_Label(t), T_Seq(then_stm, T_Seq(joinJump, T_Seq(T_Label(f), T_Seq(else_stm, T_Seq(joinJump, T_Label(join)))))))));
	}
	return Tr_noExp();
}
Tr_exp Tr_breakExp(Temp_label end) {
	T_stm s = T_Label(end);
	if (s&&s->kind == T_LABEL) {
		return Tr_Nx(T_Jump(T_Name(s->u.LABEL), Temp_LabelList(s->u.LABEL, NULL)));
	}
	assert(0);
	return Tr_noExp();
}

// move the condition check to latter part to avoid bugs out of overflowing
Tr_exp Tr_whileExp(Tr_exp cond, Temp_label end, Tr_exp body) {
	Temp_label start = Temp_newlabel(), test = Temp_newlabel();
	T_exp cond_ex = unEx(cond);
	T_stm body_nx = unNx(body);
	// return Tr_noExp();
	// todo need fix
	return Tr_Ex(T_Eseq(T_Jump(T_Name(test), Temp_LabelList(test, NULL)), T_Eseq(T_Label(start), T_Eseq(body_nx, T_Eseq(T_Label(test), T_Eseq(T_Cjump(T_eq, cond, T_Const(0), start, end), T_Eseq(T_Label(end), T_Const(0))))))));
}
Tr_exp Tr_forExp(Tr_access access,
	Temp_label end,
	Tr_exp low,
	Tr_exp high,
	Tr_exp body) {
	Temp_label start = Temp_newlabel(), test = Temp_newlabel();
	T_exp index = F_exp(access->access, T_Temp(F_FP())); // rely on hypothesis that all temp stores in reg, since F_exp pack the inherit choice u need not change it
	T_exp high_exp = unEx(high);
	T_stm body_nx = unNx(body);
	T_stm cond = T_Cjump(T_le, index, high_exp, start, end);
	T_stm mv = T_Move(index, unEx(low));
	T_stm incre = T_Move(index, T_Binop(T_plus, index, T_Const(1)));
	body_nx = T_Seq(body_nx, incre);
	// return Tr_noExp();
	// todo need fix
	return Tr_Ex(T_Eseq(mv, T_Eseq(T_Jump(T_Name(test), Temp_LabelList(test, NULL)), T_Eseq(T_Label(start), T_Eseq(body_nx, T_Eseq(T_Label(test), T_Eseq(T_Cjump(T_eq, cond, T_Const(0), start, end), T_Eseq(T_Label(end), T_Const(0)))))))));

}
Tr_exp Tr_assignExp(Tr_exp lhs, Tr_exp rhs) {
	return Tr_Nx(T_Move(unEx(lhs), unEx(rhs)));
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level) {
	return Tr_Ex(F_exp(access->access, T_Temp(F_FP())));
}
Tr_exp Tr_fieldVar(Tr_exp record, int index) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(record), T_Const(index * F_wordSize))));
}
Tr_exp Tr_subscriptVar(Tr_exp array, Tr_exp index) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(array), T_Binop(T_mul, unEx(index), T_Const(F_wordSize)))));
}
Tr_exp Tr_noExp() {
	return Tr_Ex(T_Const(0));
}

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail)
{
	Tr_expList list = (Tr_expList)checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals)
{
	F_frag pfrag = F_ProcFrag(unNx(body), level->frame);
	fragList = F_FragList(pfrag, fragList);
}

F_fragList Tr_getResult(void)
{
	F_fragList cursor = NULL, prev = NULL;
	for (cursor = stringList; cursor; cursor = cursor->tail)
		prev = cursor;
	if (prev) prev->tail = fragList;
	return stringList ? stringList : fragList;
}