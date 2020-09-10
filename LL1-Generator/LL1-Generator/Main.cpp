#include<iostream>
#include"Grammar.hpp"

int main(int argc, char* argv[]) {
	Grammar grammar = Grammar();
	grammar.readFile( "temp.txt" );
	GrammarTable table = grammar.toTableLL1();
	table.print( std::cout );
	return 0;
}
