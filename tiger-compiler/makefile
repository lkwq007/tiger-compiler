IDIR =./include
SDIR =./
TESTDIR=./test
ODIR =./obj
BDIR =./bin

COMPILER=gcc
CFLAGS=-I$(IDIR) -Wall -g -c -o
OPTIONS=-o

SEM_OBJECTS = test_semantic.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o \
	table.o prabsyn.o env.o semant.o types.o temp.o translate.o mipsframe.o tree.o assem.o printdot.o
PARSE_OBJECTS= parse_test.o parse.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o \
	table.o prabsyn.o
ASSEM_OBJECTS = main.o y.tab.o lex.yy.o errormsg.o util.o symbol.o absyn.o parse.o \
	table.o prabsyn.o env.o semant.o types.o temp.o translate.o mipsframe.o tree.o assem.o printtree.o canon.o escape.o mipscodegen.o errormsg.o

SEM_OBJS = $(patsubst %,$(ODIR)/%,$(SEM_OBJECTS))
PARSE_OBJS = $(patsubst %,$(ODIR)/%,$(PARSE_OBJECTS))
ASSEM_OBJS = $(patsubst %,$(ODIR)/%,$(ASSEM_OBJECTS))



SEM_PROG_NAME=$(BDIR)/semantest
PARSE_PROG_NAME=$(BDIR)/parsetest
ASSEM_PROG_NAME=$(BDIR)/assemtest

semantest: $(SEM_OBJS)
	$(COMPILER) $^ $(OPTIONS) $(SEM_PROG_NAME)
parsetest: $(PARSE_OBJS)
	$(COMPILER) $^ $(OPTIONS) $(PARSE_PROG_NAME)
assemtest: $(ASSEM_OBJS)
	$(COMPILER) $^ $(OPTIONS) $(ASSEM_PROG_NAME)

	
$(ODIR)/%.o: $(SDIR)/%.c
	$(COMPILER) $(CFLAGS) $@ $<
	
$(ODIR)/%.o: $(TESTDIR)/%.c
	$(COMPILER) $(CFLAGS) $@ $<

$(ODIR)/lex.yy.o: $(SDIR)/lex.yy.c
	$(COMPILER) $(CFLAGS) $@ $<

$(SDIR)/lex.yy.c: $(SDIR)/tiger.l
	flex -o $(SDIR)/lex.yy.c $<

$(ODIR)/y.tab.o: $(SDIR)/y.tab.c
	$(COMPILER) $(CFLAGS) $@ $<

$(SDIR)/y.tab.c: $(SDIR)/tiger.y
	bison -ydvo $@ $<

clean:
	rm -f $(ODIR)/*.o $(SEM_PROG_NAME) $(PARSE_PROG_NAME) $(ASSEM_PROG_NAME) $(SDIR)/lex.yy.c $(SDIR)/y.tab.c
