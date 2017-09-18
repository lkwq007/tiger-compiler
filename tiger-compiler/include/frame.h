#pragma once
#ifndef TIGER_FRAME_HEADER_
#define TIGER_FRAME_HEADER_

#include "temp.h"
#include "util.h"
#include "tree.h"

/* frame.h */
typedef struct F_frame_ *F_frame;
typedef struct F_access_*F_access;
typedef struct F_accessList_ *F_accessList;

struct F_access_ {
    enum {inFrame, inReg} kind;
    union {
        int offset; /* inFrame */
        Temp_temp reg; /* inReg */
    } u;
};
struct F_frame_ {
    Temp_label name;
    F_accessList formals;
    int local_count;
};

struct F_accessList_
{
	F_access head;
	F_accessList tail;
};


typedef struct F_frag_ *F_frag;
struct F_frag_ {
	enum {F_stringFrag, F_procFrag} kind;
	union {
		struct {
			Temp_label label;
			string str;
		} stringg;
		struct {
			T_stm body;
			F_frame frame;
		} proc;
	} u;
};

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ {
	F_frag head;
	F_fragList tail;
};

F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);
F_fragList F_FragList(F_frag head, F_fragList tail);

Temp_map F_tempMap;
Temp_tempList F_registers(void);
string F_getlabel(F_frame f);
T_exp F_exp(F_access acc, T_exp framePtr);
F_access F_allocLocal(F_frame f, bool escape);
F_accessList F_formals(F_frame f);
Temp_label F_name(F_frame f);
extern const int F_wordSize;

Temp_temp F_FP(void);
Temp_temp F_RV(void);
Temp_temp F_SP(void);
Temp_temp F_RA(void);
Temp_temp F_ZERO(void);

F_frame F_newFrame(Temp_label name, U_boolList formals);
T_exp F_externalCall(string str, T_expList args);
T_stm F_procEntryExit1(F_frame frame, T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList_ body);
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);

bool F_doesEscape(F_access access);
Temp_map F_tempMap;

#endif