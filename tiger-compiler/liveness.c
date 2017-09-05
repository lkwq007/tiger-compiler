#include "temp.h"
#include "graph.h"
#include "flowgraph.h"
#include "liveness.h"
#include "util.h"
// live at some node
static void enterLiveMap(G_table t, G_node flownode, Temp_tempList temps)
{
	G_enter(t, flownode, temps);
}

static Temp_tempList lookupLiveMap(G_table t, G_node flownode)
{
	return (Temp_tempList)G_look(t, flownode);
}

static Temp_tempList insertTempList(Temp_temp temp, Temp_tempList list)
{
	Temp_tempList new_list, prev;
	if (list == NULL||(unsigned)temp<(unsigned)(list->head))
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

static Temp_tempList unionTempList(Temp_tempList l1, Temp_tempList l2)
{
	Temp_tempList list=NULL;
	while (l1)
	{
		list=insertTempList(l1->head, list);
		l1 = l1->tail;
	}
	while (l2)
	{
		list = insertTempList(l2->head, list);
	}
	return list;
}

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

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail)
{
	Live_moveList p = (Live_moveList)checked_malloc(sizeof *p);
	p->src = src;
	p->dst = dst;
	p->tail = tail;
	return p;
}
// the temporary is represented by n
Temp_temp Live_gtemp(G_node n)
{
	return G_nodeInfo(n);
}
// flow: flowgraph
// liveness analysis

struct Live_graph Live_liveness(G_graph flow)
{
	// iter through the flow graph
	// maintain two Graph table of live-in and live-out
	G_table in_table = G_empty();
	G_table out_table = G_empty();
	G_nodeList list;
	int all_same;
	while (1)
	{
		all_same = 1;
		list = G_nodes(flow);
		Temp_tempList in, out, in_next, out_next;
		for (; list; list = list->tail)
		{
			
		}
	}
}