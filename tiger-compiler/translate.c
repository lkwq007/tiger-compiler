/* translate.c */
#include "translate.h"
#include "frame.h"
#include "temp.h"
#include "tree.h"
#include "absyn.h"


struct Tr_access_
{
    Tr_level level;
    F_access access;
};

struct Tr_level_
{
    Tr_level parent;
    Temp_label name;
    F_frame frame;
    Tr_accessList formals;
};

struct patchList_ {
    Temp_label *head;
    patchList tail;
};

struct Tr_accessList_ {
    Tr_access head;
    Tr_accessList tail;
};

typedef struct Cx {
    patchList trues;
    patchList falses;
    T_stm stm;
} *Cx;

struct Tr_exp_ {
    enum {Tr_ex, Tr_nx, Tr_cx} kind;
    union {
        T_exp ex;
        T_stm nx;
        Cx cx;
    } u;
};

struct Tr_expList_ {
    Tr_exp head;
    Tr_expList tail;
};

static Tr_level outer = NULL;
F_fragList stringList = NULL;

Tr_access Tr_Access(Tr_level level, F_access access) {
    Tr_access list = checked_malloc(sizeof(*list));
    list->level = level;
    list->access = access;
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
    Tr_accessList list = checked_malloc(sizeof(*list));
    list->head = head;
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
    Cx cx;
    cx->trues = trues;
    cx->falses = falses;
    cx->stm = stm;
    res->u.cx = cx;
    return res;
}


static T_exp unEx(Tr_exp e)
{
    switch(e->kind) {
    case Tr_ex:
        return e->u.ex;
    case Tr_nx:
        return T_Eseq(e->u.nx, T_Const(0));
    case Tr_cx: {
        Temp_temp r = Temp_newtemp();
        Temp_label t = Temp_newlabel(), f = Temp_newlabel();
        doPatch(e->u.cx.trues, t);
        doPatch(e->u.cx.falses, f);
        return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
                      T_Eseq(e->u.cx.stm,
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
    switch(e->kind) {
    case Tr_ex:
        return T_Exp(e->u.ex);
    case Tr_nx:
        return e->u.nx;
    case Tr_cx: {
        Temp_temp r = Temp_newtemp();
        Temp_label t = Temp_newlabel(), f = Temp_newlabel();
        doPatch(e->u.cx.trues, t);
        doPatch(e->u.cx.falses, f);
        return T_Exp(T_Eseq(T_Move(T_Temp(r), T_Const(1)),
                            T_Eseq(e->u.cx.stm,
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
    switch(e->kind) {
    case Tr_ex: {
        Cx cx;
        /* If comparison yields true then the expression was false (compares equal
         * to zero) so we jump to false label. */
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
    Tr_accessList head=NULL, tail=NULL;
    // F_accessList f_list = frame->formals->tail;
    for(F_accessList f_list = frame->formals->tail; f_list; f_list = f_list->tail)
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
    Temp_label newLabel = Temp_newLabel();
    Tr_exp expr = T_Name(newLabel);
    stringList = F_FragList(F_StringFrag(newLabel, str), stringList);
    return Tr_Ex(expr);
}

T_expList translateToTreeExpList(Tr_expList argList) {
    T_expList res = NULL;
    while(argList) {
        Tr_exp temp = argList->head;
        res = T_expListAppend(res, unEx(temp));
        argList = argList->tail;
    }
    return res;
}

Tr_exp Tr_callExp(Tr_level level, Temp_label label, Tr_expList argList) {
    T_expList tempList = translateToTreeExpList(argList);
    return Tr_Ex(T_Call(T_Name(label), tempList));
}
Tr_exp Tr_opExp(int op, Tr_exp left, Tr_exp right) {
    T_binOp oper;
    switch(op) {
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
    switch(op) {
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
Tr_exp Tr_stringRelExp(int op, Tr_exp left, Tr_exp right){
    return F_externalCall("stringCompare", T_ExpList(left, T_ExpList(right, NULL)));
}
Tr_exp Tr_recordExp(Tr_expList fields, int size);
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);
Tr_exp Tr_seqExp(Tr_expList stmts);
Tr_exp Tr_ifExp(Tr_exp cond, Tr_exp then, Tr_exp else_);
Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp body);
Tr_exp Tr_forExp(Tr_access access,
                 Tr_exp low,
                 Tr_exp high,
                 Tr_exp body);
Tr_exp Tr_assignExp(Tr_exp lhs, Tr_exp rhs){
    return Tr_Nx(T_Move(unEx(lhs), unEx(rhs)));
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level) {
    return Tr_Ex(F_Exp(access->access->u.reg));
}
Tr_exp Tr_fieldVar(Tr_exp record, int index){
    return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(record), T_Const(index * F_wordSize))));
}
