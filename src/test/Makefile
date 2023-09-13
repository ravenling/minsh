CC=g++
CFLAGS= -g -Werror -Wall 
TARGET=parser_test.out
SRCS=../parser/lex.cpp ../parser/parser.cpp ../parser/prs.tab.cc parser_test.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@
	
clean:
	rm -f *.out