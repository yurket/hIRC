CC      = gcc
LDFLAGS = -lfl


msg_lexer: lex.yy.o
	$(CC) -o msg_lexer lex.yy.o ${LDFLAGS}

lex.yy.o: lex.yy.c
	$(CC) -c lex.yy.c

lex.yy.c: messages.flex
	flex messages.flex

clean:
	rm lex.yy.o
	rm msg_lexer
