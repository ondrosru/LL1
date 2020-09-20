#include<iostream>
#include"Grammar.hpp"

int main(int argc, char* argv[]) {
	Grammar grammar = Grammar();
	grammar.readFile( "test2.txt" );
	//grammar.write( std::cout );
	GrammarTable table = grammar.toTableLL1();
	table.print( std::cout );
	//grammar.write( std::cout );
	return 0;
}
