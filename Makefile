all: simulacion

simulacion: parser.tab.o lex.yy.o simulacion.o
	gcc -o simulacion parser.tab.o lex.yy.o simulacion.o -lfl

parser.tab.o: parser.tab.c parser.tab.h
	gcc -c parser.tab.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

simulacion.o: simulacion.c
	gcc -c simulacion.c

parser.tab.c parser.tab.h: parser.y
	bison -d -o parser.tab.c parser.y

lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f parser.tab.c parser.tab.h lex.yy.c *.o