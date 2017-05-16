/* mipsframe.c */
#include "frame.h"

struct F_access_
{
	enum { inFrame, inReg } kind;
	union
	{
		int offset;
		Temp_temp reg;
	} u;
};
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
