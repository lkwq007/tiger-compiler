#include "temp.h"
#include "graph.h"
#include "assem.h"
#include "flowgraph.h"

Temp_tempList FG_def(G_node n)
{
	AS_instr inst;
	inst = (AS_instr)G_nodeInfo(n);
	switch (inst->kind)
	{
	case I_LABEL:
		return NULL;
	case I_MOVE:
		return inst->u.MOVE.dst;
	case I_OPER:
		return inst->u.OPER.dst;
	}
}

Temp_tempList FG_use(G_node n)
{
	AS_instr inst;
	inst = (AS_instr)G_nodeInfo(n);
	switch (inst->kind)
	{
	case I_LABEL:
		return NULL;
	case I_MOVE:
		return inst->u.MOVE.src;
	case I_OPER:
		return inst->u.OPER.src;
	}
}

bool FG_isMove(G_node n)
{
	AS_instr inst;
	inst = (AS_instr)G_nodeInfo(n);
	if (inst->kind == I_MOVE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
// just store instr in node
G_graph FG_AssemFlowGraph(AS_instrList il)
{
	AS_instrList list = il;
	G_node prev = NULL, current = NULL;
	G_nodeList operlist = NULL, temp = NULL;
	G_graph graph = G_Graph();
	S_table entry = S_empty();
	while (list)
	{
		prev = current;
		current = G_Node(graph, list->head);
		if (prev)
		{
			G_addEdge(prev, current);
		}
		switch (list->head->kind)
		{
		case I_OPER:
		{
			// record nodes with multi succ
			if (list->head->u.OPER.jumps)
			{
				operlist = G_NodeList(current, operlist);
			}
			break;
		}
		case I_LABEL:
		{
			S_enter(entry, list->head->u.LABEL.label, current);
			break;
		}
		case I_MOVE:
		{
			break;
		}
		default:
			assert(0);
			break;
		}
		list = list->tail;
	}
	temp = operlist;
	while (temp)
	{
		G_node next;
		Temp_labelList target = ((AS_instr)G_nodeInfo(temp->head))->u.OPER.jumps->labels;
		for (; target; target = target->tail)
		{
			next = S_look(entry, target->head);
			// TODO shall I judge whether next is NULL?
			G_addEdge(temp->head, next);
		}
		temp = temp->tail;
	}
	return graph;
}