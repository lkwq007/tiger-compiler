#include "semant.h"
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "printtree.h"
#include <stdio.h>
#include <stdlib.h>

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname)
{
	EM_reset(fname);
	if (yyparse() == 0)	/* parsing worked */
		return absyn_root;
	else
		return NULL;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: semantest filename\n");
		exit(1);
	}
	F_fragList frags;
	int isFirstString = 1;
	if (parse(argv[1])) frags = SEM_transProg(absyn_root);
	else assert(0);
	for (;frags;frags=frags->tail) {
		if (frags->head->kind == F_procFrag) {
			printStmList(stdout, T_StmList(frags->head->u.proc.body, NULL));
		} else if (frags->head->kind == F_stringFrag) {
			if (isFirstString) {
				fprintf(stdout, "section .text\n\"\t%s\t.db\t\"%s\"\n",
						frags->head->u.stringg.str,
						frags->head->u.stringg.label->name);
				isFirstString = 0;
			} else {
				fprintf(stdout, "\t%s\t.db\t\"%s\"\n", 
						frags->head->u.stringg.str,
						frags->head->u.stringg.label->name);
			}
		}
	}
	return 0;
}
