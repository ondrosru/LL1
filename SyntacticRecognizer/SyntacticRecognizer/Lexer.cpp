#include "Lexer.h"
#include "StateReader.h"
#include "Keywords.h"
#include <algorithm>




Lexer::Lexer( const std::string& fileName )
	:m_file( fileName ),
	m_row( 1 ),
	m_col( 1 )
{
	m_file >> m_currentChar;
	StateReader reader = StateReader( "LexerMachine.csv" );
	m_startState = reader.GetStartState();
}

Lexer::~Lexer() {}

Token Lexer::GetToken()
{
	m_currentState = m_startState;
	std::string value;
	size_t col = m_col;
	while ( !m_file.eof() )
	{
		if ( StringToTokenType( m_currentState->GetName() ) == TokenType::EndLine )
		{
			m_currentState = m_startState;
			value = "";
			m_row++;
			m_col = 1;
			col = 1;
		}
		if ( m_currentState == m_startState )
		{
			m_col = col;
		}
		std::shared_ptr<State> nextState = m_currentState->GetNextState( m_currentChar );
		if ( nextState == NULL )
		{
			size_t temp = m_col;
			m_col = col;
			if ( StringToTokenType( m_currentState->GetName() ) == TokenType::Identifier && KEYWORDS.find( value ) != KEYWORDS.end() )
			{
				return Token{ value, TokenType::Keyword, m_row, temp };
			}
			if ( m_currentState != m_startState )
			{
				if ( StringToTokenType( m_currentState->GetName() ) == TokenType::Integer && value.length() > 8 )
				{
					return Token{ value, TokenType::Unexpected, m_row, temp };
				}
				return Token{ value, StringToTokenType( m_currentState->GetName() ), m_row, temp };
			}
		}
		else
		{
			m_currentState = nextState;
		}
		if ( (m_currentChar != ' ' || m_currentState->GetName() != START_STATE_NAME ) && StringToTokenType( m_currentState->GetName() ) != TokenType::EndLine )
		{
			value += m_currentChar;
		}
		if ( m_currentState->GetName() == COMMENT_STATAE_NAME )
		{
			char ch;
			m_file.get( ch );
			while ( ch != '\n' )
			{
				m_file.get( ch );
			}
			m_currentState = m_startState;
			m_currentState = m_currentState->GetNextState( ch );
		}
		if ( m_currentState->GetName() == MULTI_COMMENT_STATAE_NAME )
		{
			value = "";
			char ch;
			m_file.get( ch );
			m_col++;
			while ( m_currentState->GetName() != "endMultiComment" && m_currentState->GetName() != "end" )
			{
				std::shared_ptr<State> temp = m_currentState;
				m_currentState = m_currentState->GetNextState( ch );
				if ( m_currentState == NULL )
				{
					m_currentState = temp;
				}
				if ( m_currentState->GetName() == "endLine" )
				{
					m_currentState = temp;
					m_row++;
					m_col = 1;
				}
				if ( m_currentState->GetName() != MULTI_COMMENT_STATAE_NAME )
				{
					m_file.get( ch );
					m_currentState = m_currentState->GetNextState( ch );
					if ( m_currentState == NULL )
					{
						m_currentState = temp;
					}
				}
				m_file.get( ch );
				if ( m_file.eof() )
				{
					return Token{ value, TokenType::End, m_row, m_col };
				}
				m_col++;
			}
			col = m_col;
			m_currentState = m_startState;
			if ( m_currentState->GetNextState( ch ) != NULL )
			{
				m_currentState = m_currentState->GetNextState( ch );
			}
		}
		m_file.get( m_currentChar );
		col++;
	}
	if ( StringToTokenType( m_currentState->GetName() ) == TokenType::Identifier && KEYWORDS.find( value ) != KEYWORDS.end() )
	{
		return Token{ value, TokenType::Keyword, m_row, m_col };
	}
	if ( m_currentState != m_startState )
	{
		return Token{ value, StringToTokenType( m_currentState->GetName() ), m_row, m_col };
	}
	m_col = col;
	return Token{ value, TokenType::End, m_row, m_col };
}

TokenType Lexer::StringToTokenType( std::string name )
{
	std::transform( name.begin(), name.end(), name.begin(), tolower );
	TokenType type;
	if ( "identifier" == name )
	{
		type = TokenType::Identifier;
	}
	else if ( "integer" == name ) {
		type = TokenType::Integer;
	}
	else if ( "integer1" == name ) {
		type = TokenType::Integer;
	}
	else if ( "integer2" == name ) {
		type = TokenType::Integer;
	}
	else if ( "binarynumber" == name ) {
		type = TokenType::BinaryNumber;
	}
	else if ( "float" == name ) {
		type = TokenType::Float;
	}
	else if ( "hexnumber" == name ) {
		type = TokenType::HexNumber;
	}
	else if ( "bigfloat" == name )
	{
		type = TokenType::BigFloat;
	}
	else if ( "char" == name )
	{
		type = TokenType::Char;
	}
	else if ( "string" == name )
	{
		type = TokenType::String;
	}
	else if ( "leftroundbracket" == name )
	{
		type = TokenType::LeftRoundBracket;
	}
	else if ( "rightroundbracket" == name )
	{
		type = TokenType::RightRoundBracket;
	}
	else if ( "leftsquarebracket" == name )
	{
		type = TokenType::LeftSquareBracket;
	}
	else if ( "rightsquarebracket" == name )
	{
		type = TokenType::RightSquareBracket;
	}
	else if ( "leftcurlybracket" == name )
	{
		type = TokenType::LeftCurlyBracket;
	}
	else if ( "rightcurlybracket" == name )
	{
		type = TokenType::RightCurlyBracket;
	}
	else if ( "lessthan" == name )
	{
		type = TokenType::LessThan;
	}
	else if ( "greaterthan" == name )
	{
		type = TokenType::GreaterThan;
	}
	else if ( "equal" == name )
	{
		type = TokenType::Equal;
	}
	else if ( "assignment" == name )
	{
		type = TokenType::Assignment;
	}
	else if ( "plus" == name )
	{
		type = TokenType::Plus;
	}
	else if ( "minus" == name )
	{
		type = TokenType::Minus;
	}
	else if ( "asterisk" == name )
	{
		type = TokenType::Asterisk;
	}
	else if ( "slash" == name )
	{
		type = TokenType::Slash;
	}
	else if ( "backslash" == name )
	{
		type = TokenType::Backslash;
	}
	else if ( "dot" == name )
	{
		type = TokenType::Dot;
	}
	else if ( "comma" == name )
	{
		type = TokenType::Comma;
	}
	else if ( "colon" == name )
	{
		type = TokenType::Colon;
	}
	else if ( "semicolon" == name )
	{
		type = TokenType::Semicolon;
	}
	else if ( "end" == name )
	{
		type = TokenType::End;
	}
	else if ( "endline" == name )
	{
		type = TokenType::EndLine;
	}
	else
	{
		type = TokenType::Unexpected;
	}
	return type;
}
