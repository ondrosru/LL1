#include <iomanip>
#include <iostream>
#include "SyntaxCheck.hpp"


int main(int argc, char **argv) {
  setlocale(LC_ALL, "Russian");
  //SyntaxTable table( "SyntaxTable.txt" );
  SyntaxCheck syntaxCheck( "SyntaxTable.txt", "Source.txt" );
  std::string str = syntaxCheck.Check();
  std::cout << str << std::endl;
}
