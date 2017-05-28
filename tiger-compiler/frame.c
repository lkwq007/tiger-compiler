#include "frame.h"
#include "temp.h"
#include "tree.h"
#include "util.h"


F_access InReg(Temp_temp reg);


static F_access F_Access(Temp_temp reg) {
    F_access res = checked_malloc(sizeof(*res));
    res->kind = inReg;
    res->u.reg = reg;
    return res;
}

static F_accessList F_AccessList(F_access head, F_accessList tail) {
    F_accessList res = checked_malloc(sizeof(*res));
    res->head = head;
    res->tail = tail;
    return res;
}

F_accessList makeFormalAccessList(U_boolList formals) {
    U_boolList fmls;
    F_accessList headList = NULL, tailList = NULL;
    int i = 0;
    for (fmls = formals; fmls; fmls = fmls->tail, i++) {
        F_access access = NULL;
        // if (i < F_K && !fmls->head) {
        access = InReg(Temp_newtemp());
        // } else {
        /* Keep a space for return address space. */
        // access = InFrame((2 + i) * F_wordSize);
        // }
        if (headList) {
            tailList->tail = F_AccessList(access, NULL);
            tailList = tailList->tail;
        } else {
            headList = F_AccessList(access, NULL);
            tailList = headList;
        }
    }
    return headList;
}

F_access InReg(Temp_temp reg) {
    return F_Access(reg);
}

F_frame F_newFrame(Temp_label name, U_boolList formals)
{
    F_frame f = checked_malloc(sizeof(*f));
    f->name = name;
    f->formals = makeFormalAccessList(formals);
    f->local_count = 0;
    return f;
}

T_exp F_exp(F_access acc, T_exp framePtr) {
    return T_Temp(acc->u.reg);
}


Temp_label F_name(F_frame f) {
    return f->name;
}

F_accessList F_formals(F_frame f) {
    return f->formals;
}

F_access F_allocLocal(F_frame f, bool escape) {
    f->local_count++;
    return InReg(Temp_newtemp());
}

F_frag F_StringFrag(Temp_label label, string str) {
    F_frag res = checked_malloc(sizeof(*res));
    res->kind = F_stringFrag;
    res->u.stringg.label = label;
    res->u.stringg.str = str;
    return res;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
    F_frag res = checked_malloc(sizeof(*res));
    res->kind = F_procFrag;
    res->u.proc.body = body;
    res->u.proc.frame = frame;
    return res;
}

F_fragList F_FragList(F_frag head, F_fragList tail) {
    F_fragList res = checked_malloc(sizeof(*res));
    res->head = head;
    res->tail = tail;
    return res;
}


T_stm F_procEntryExit1(F_frame frame, T_stm stm) {
    return stm;
}

T_exp F_externalCall(string str, T_expList args) {
    return T_Call(T_Name(Temp_namedlabel(str)), args);
}

static Temp_temp fp = NULL;
Temp_temp F_FP(void){
    if(!fp)
        fp = Temp_newtemp();;
    return fp;
}
const int F_wordSize=8;