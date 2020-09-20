#include<iostream>
#include"Grammar.hpp"

int main(int argc, char* argv[]) {
	Grammar grammar = Grammar();
	grammar.readFile( "test2.txt" );
	std::ofstream outFile( "out.txt" );
	//grammar.write( std::cout );
	GrammarTable table = grammar.toTableLL1();
	table.print( outFile );
	//grammar.write( std::cout );
	return 0;
}
