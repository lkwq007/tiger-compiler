#include "frame.h"
#include "assem.h"
#include "temp.h"
#include "graph.h"
#include "color.h"
#include "flowgraph.h"
#include "liveness.h"
#include "regalloc.h"
#include "util.h"
// node list & set
// I think precolored & initial can be temp_templist
// and we can use G_graph to represent adjSet, and attach adjList & degree & temp into G_node
// so, other workList can be represented by 
typedef struct workList_ *workList;
enum workType
{
	W_SIMPLIFY,
	W_FREEZE,
	W_SPILL,
	W
};
struct workList_
{
	G_node head;
	workList prev;
	workList tail;
};
// the entry for registers allocation
// precedure liveness-analysis -> build interference map and bit matrix -> make worklist
// -> simplify -> coalesce -> freeze -> spill -> assign color -> rewrite program
// RA_regAlloc rec
static workList simplifyWorkList = NULL;
static workList spillWorkList = NULL;
static workList freezeWorkList = NULL;

static workList selectStack = NULL;


static AS_instrList rewriteProgram(F_frame f, AS_instrList il, Temp_tempList spills)
{
	for (; spills; spills = spills->tail)
	{
		Temp_temp v = spills->head;
		F_access access;
		AS_instrList list;
		access = F_allocLocal(f, TRUE);
		for (; list; list = list->tail)
		{
			
		}
	}
}

static void makeWorkList()
{
	Temp_tempList initial,list;
	Temp_temp temp;
	for (; initial; initial = initial->tail)
	{
		G_node node;
		list = initial;
		temp = initial->head;
		node=
	}
}



struct RA_result RA_regAlloc(F_frame f, AS_instrList il)
{
	G_graph flow = FG_AssemFlowGraph(il);
	struct Live_graph live_graph = Live_liveness(flow);
	struct COL_result col_result = COL_color(live_graph.graph, initial, regs);
	if (col_result.spills != NULL)
	{
		il = RewriteProgram(il, col_result.spills);
		return RA_regAlloc(f, il);
	}
	// return the final result
	struct RA_result ra_result;
	ra_result.coloring = col_result.coloring;
	ra_result.il = il;
	return ra_result;
}