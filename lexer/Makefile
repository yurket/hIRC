CC      = gcc
CXX     = g++
LDFLAGS = -lfl
CXX_FLAGS = -std=c++11

msg_lexer: lex.yy.o
	$(CC) -o msg_lexer lex.yy.o ${LDFLAGS}

lex.yy.o: lex.yy.c
	$(CC) -c lex.yy.c

lex.yy.c: messages.flex
	flex messages.flex


regex_test: regex_test.cc message.cc message.h
	$(CXX) -o regex_test regex_test.cc message.cc ${CXX_FLAGS}


clean:
	rm regex_test
	rm lex.yy.o
	rm msg_lexer
