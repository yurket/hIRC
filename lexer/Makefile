CC      = gcc
LDFLAGS = -lfl

msg_lexer: lex.yy.c
	flex messages.flex
	$(CC) -o msg_lexer lex.yy.c ${LDFLAGS}

# clean: lex.yy.o
# 	rm lex.yy.c
# 	rm msg_lexer
