#	Christian Cyr
#	3/23/2022
#	Makefile is created to compile all Lab6 files if any changes are made to
#	the AST.c AST.h Lab6.l or Lab6.y. symtables aren't used yet
#	Saved as lab6.
#
all:    Lab9

Lab9:  Lab9.y Lab9.l symtable.h symtable.c AST.h AST.c emit.c emit.h
	yacc -d Lab9.y
	lex Lab9.l
	gcc y.tab.c lex.yy.c symtable.c AST.c emit.c -o Lab9

clean:
	rm -f Lab9

run: Lab9
	./Lab9
