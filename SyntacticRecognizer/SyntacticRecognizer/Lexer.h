#pragma once
#include <fstream>
#include "State.h"
#include "Token.h"
class Lexer
{
public:
	const std::string START_STATE_NAME = "start";
	const std::string COMMENT_STATAE_NAME = "comment";
	const std::string MULTI_COMMENT_STATAE_NAME = "multiComment";
	Lexer( const std::string& fileName );
	~Lexer();
	Token GetToken();
private:
	std::ifstream m_file;
	std::shared_ptr<State> m_startState;
	std::shared_ptr<State> m_currentState;
	char m_currentChar;
	size_t m_row;
	size_t m_col;
	TokenType StringToTokenType(std::string name);
};

