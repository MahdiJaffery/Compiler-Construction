all: cleanBefore lexer parser tac

lexer:
	g++ Lexer.cpp -o 1
	./1

parser:
	g++ Parser.cpp -o 2
	./2

tac:
	g++ TacGen.cpp -o 3
	./3

clean:
	rm -f 1 2 *.out *Table.txt Tokens.txt Lexer Parser

cleanBefore:
	rm -f *Table.txt Tokens.txt

.PHONY: all lexer clean parser 