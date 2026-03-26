all:
	bison -d parser/parser.y
	flex lexer/lexer.l
	gcc -o compilador src/main.c parser.tab.c lex.yy.c -lfl

clean:
	rm -f compilador lex.yy.c *.tab.c *.tab.h