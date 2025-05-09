all: cleanBefore lexer parser tac

lexer:
	rm -f 1 *.out *Table.txt Tokens.txt Lexer
	g++ Lexer.cpp -o 1
	./1

parser:
	rm -f 2 Parser
	g++ Parser.cpp -o 2
	./2

tac:
	rm -f 3 Tac.txt TacGen
	g++ TacGen.cpp -o 3
	./3

clean:
	rm -f 1 2 3 *.out *Table.txt Tokens.txt Tac.txt Lexer Parser TacGen

cleanBefore:
	rm -f *Table.txt Tokens.txt

.PHONY: all lexer clean parser 