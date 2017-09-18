#include "util.h"
#include "assem.h"
#include "temp.h"
#include "graph.h"
#include "flowgraph.h"
#include "table.h"
#include "worklist.h"
#include "liveness.h"
// map node -> tempList
static void enterLiveMap(G_table t, G_node flownode, Temp_tempList temps)
{
	G_enter(t, flownode, temps);
}

// lookup node -> tempList
static Temp_tempList lookupLiveMap(G_table t, G_node flownode)
{
	return (Temp_tempList)G_look(t, flownode);
}

// insert temp into list, will return new ordered list
static Temp_tempList insertTempList(Temp_temp temp, Temp_tempList list)
{
	Temp_tempList new_list, prev;
	if (list == NULL || (unsigned)temp < (unsigned)(list->head))
	{
		new_list = Temp_TempList(temp, list);
	}
	else
	{
		new_list = list;
		prev = list;
		for (list = list->tail; list; list = list->tail)
		{
			if (temp == list->head)
			{
				return new_list;
			}
			else if ((unsigned)temp > (unsigned)(list->head))
			{
				break;
			}
		}
		prev->tail = Temp_TempList(temp, list);
	}
	return new_list;
}

// union two tempList, will return a new ordered list
static Temp_tempList unionTempList(Temp_tempList l1, Temp_tempList l2)
{
	Temp_tempList list = NULL;
	while (l1)
	{
		list = insertTempList(l1->head, list);
		l1 = l1->tail;
	}
	while (l2)
	{
		list = insertTempList(l2->head, list);
	}
	return list;
}

// diff l1 l2, will return a new ordered list
static Temp_tempList diffTempList(Temp_tempList l1, Temp_tempList l2)
{
	Temp_tempList list = NULL, temp = NULL, prev;
	for (; l1; l1 = l1->tail)
	{
		if (list == NULL)
		{
			list = Temp_TempList(l1->head, NULL);
			temp = list;
		}
		else
		{
			temp->tail = Temp_TempList(l1->head, NULL);
			temp = temp->tail;
		}
	}
	temp = list;
	prev = list;
	while (temp&&l2)
	{
		if (temp->head == l2->head)
		{
			if (list == temp)
			{
				list = list->tail;
				free(temp);
				temp = list;
				prev = list;
			}
			else
			{
				prev->tail = temp->tail;
				free(temp);
				temp = prev->tail;
			}
			l2 = l2->tail;
		}
		else if ((unsigned)(temp->head) < (unsigned)(l2->head))
		{
			prev = temp;
			temp = temp->tail;
		}
		else
		{
			l2 = l2->tail;
		}
	}
	return list;
}

// compare two tempList, if same then true
static bool sameTempList(Temp_tempList l1, Temp_tempList l2)
{
	while (l1&&l2)
	{
		if (l1->head != l2->head)
		{
			return FALSE;
		}
	}
	if (l1 || l2)
	{
		return FALSE;
	}
	return TRUE;
}

// free the whole list
static void freeTempList(Temp_tempList list)
{
	Temp_tempList temp;
	while (list)
	{
		temp = list;
		list = list->tail;
		free(temp);
	}
	return;
}

// constructor for Live_moveList
Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail)
{
	Live_moveList p = (Live_moveList)checked_malloc(sizeof *p);
	p->src = src;
	p->dst = dst;
	p->tail = tail;
	return p;
}

// the temporary is represented by n
tempInfo Live_gtemp(G_node n)
{
	return G_nodeInfo(n);
}

// liveness analysis with flowgraph
// the pseudo codes for register reallocation in the textbook were kind of misunderstanding
// I combined liveness-analysis and build procedure
struct Live_graph Live_liveness(G_graph flow)
{
	// iter through the flow graph
	// maintain two Graph table of live-in and live-out
	G_table in_table = G_empty();
	G_table out_table = G_empty();
	G_nodeList list;
	bool all_same;
	// construct liveness map
	while (1)
	{
		all_same = TRUE;
		list = G_nodes(flow);
		G_nodeList succ;
		Temp_tempList in, out, in_next, out_next, def, use, temp, in_succ;
		for (; list; list = list->tail)
		{
			// in out
			in = lookupLiveMap(in_table, list->head);
			out = lookupLiveMap(out_table, list->head);
			// def use
			def = FG_def(list->head);
			use = FG_use(list->head);
			// in[n] = use[n] U ( out[n] - def[n] )
			temp = diffTempList(out, def);
			in_next = unionTempList(temp, use);
			freeTempList(temp);
			// out[n] U ( in[s] )
			succ = G_succ(list->head);
			out_next = NULL;
			for (; succ; succ = succ->tail)
			{
				in_succ = lookupLiveMap(in_table, succ->head);
				temp = out_next;
				out_next = unionTempList(out_next, in_succ);
				freeTempList(temp);
			}
			if (!(sameTempList(in, in_next) && sameTempList(out, out_next)))
			{
				all_same = FALSE;
			}
			freeTempList(in);
			freeTempList(out);
			enterLiveMap(in_table, list->head, in_next);
			enterLiveMap(out_table, list->head, out_next);
		}
		if (all_same)
		{
			break;
		}
	}
	list = G_nodes(flow);
	T_table temp_table = T_empty();
	G_graph interference = G_Graph();
	//Live_moveList moveList=NULL;
	moveList worklistMoves=NULL;
	// construct interference graph
	for (; list; list = list->tail)
	{
		Temp_tempList def, live;
		live = lookupLiveMap(out_table, list->head);
		def = FG_def(list->head);
		// usually, def(n) contain 0 or 1 temp, but to cover all cases, I iter the def
		for (; def; def = def->tail)
		{
			G_node node, adj;
			node = T_look(temp_table, def->head);
			if (node == NULL)
			{
				tempInfo info=TempInfo(def->head);
				node = G_Node(interference, info);
				T_enter(temp_table, def->head, node);
			}
			Temp_tempList out;
			out = lookupLiveMap(out_table, list->head);
			for (; out; out = out->tail)
			{
				adj = T_look(temp_table, out->head);
				if (adj == NULL)
				{
					tempInfo info = TempInfo(out->head);
					adj = G_Node(interference, info);
					T_enter(temp_table, out->head, adj);
				}
				if (FG_isMove(list->head))
				{
					worklistMoves = InsertMoveList(worklistMoves,MoveInstr());
					Temp_temp src = ((AS_instr)G_nodeInfo(list->head))->u.MOVE.src->head;
					if (src == out->head)
					{
						//moveList = Live_MoveList(adj, node, moveList);
						continue;
					}
				}
				G_addEdge(node, adj);
				G_addEdge(adj, node);
			}
		}
	}
	struct Live_graph live_graph;
	live_graph.graph = interference;
	//live_graph.moves = moveList;
	live_graph.moves = NULL;
	return live_graph;
}