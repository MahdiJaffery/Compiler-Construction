all: lexer parser

lexer:
	g++ Lexer.cpp -o 1
	./1

parser:
	g++ Parser.cpp -o 2
	./2

clean:
	rm -f 1 2 *.out *Table.txt Tokens.txt

.PHONY: all lexer clean parser 