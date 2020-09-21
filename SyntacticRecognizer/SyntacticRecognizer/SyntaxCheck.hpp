#include <stack>
#include <string>
#include "SyntaxTable.hpp"
#include "Lexer.h"

class SyntaxCheck {
public:
	SyntaxCheck( std::string syntaxTablePath, std::string sourceFilePath )
		:m_syntaxTable( syntaxTablePath ),
		m_lexer( sourceFilePath )
	{
	};
	~SyntaxCheck() {};

	std::string Check() {
		m_currentTableIndex = 1;
		Row currRow = m_syntaxTable.getRow( m_currentTableIndex );
		m_currentToken = m_lexer.GetToken();
		while ( true ) {
			while ( m_currentToken.type == TokenType::EndLine && !checkRow( currRow ) ) {
				m_currentToken = m_lexer.GetToken();
			}
			if ( checkRow( currRow ) ) {
				isShift( currRow );
				isStack( currRow );

				if ( currRow.next != -1 ) {
					m_currentTableIndex = currRow.next;
					currRow = m_syntaxTable.getRow( m_currentTableIndex );
				}
				else {
					if ( currRow.next == -1 && m_stack.size() > 0 ) {
						m_currentTableIndex = m_stack.top();
						m_stack.pop();
						currRow = m_syntaxTable.getRow( m_currentTableIndex );
					}
					else if (currRow.next == -1 && currRow.endState) {
						return "OK";
					}
				}
			}
			else if ( !currRow.error ) {
				m_currentTableIndex++;
				currRow = m_syntaxTable.getRow( m_currentTableIndex );
			}
			else {
				std::string error;
				error = "ERROR: ";
				error += "Ошибка в ";
				error += std::to_string( m_currentToken.row);
				error += ":";
				error += std::to_string(m_currentToken.col);
				error += "\nОжидалось: ";
				for ( std::set<std::string>::iterator it = currRow.guidingSets.begin(); it != currRow.guidingSets.end(); it++ ) {
					if ( it != currRow.guidingSets.begin() ) {
						error += " or ";
					}
					error += *it; 
				}
				error += "\nВстретился: ";
				error += m_currentToken.value;
				if ( m_currentToken.type == TokenType::End ) {
					error += "EndFile";
				}
				return error;
			}
		}
	}

private:
	unsigned int m_currentTableIndex = 1;
	Token m_currentToken;
	Lexer m_lexer;
	std::stack<unsigned int> m_stack;
	SyntaxTable m_syntaxTable;

	std::string convertToString( TokenType type ) {
		std::string str;
		switch ( type ) {
		case TokenType::Identifier:
			str = "identifier";
			break;
		case TokenType::Integer:
			str = "integer";
			break;
		case TokenType::Float:
			str = "float";
			break;
		case TokenType::HexNumber:
			str = "hexnumber";
			break;
		case TokenType::BigFloat:
			str = "bigfloat";
			break;
		case TokenType::BinaryNumber:
			str = "binarynumber";
			break;
		case TokenType::Char:
			str = "char";
			break;
		case TokenType::String:
			str = "string";
			break;
		case TokenType::Keyword:
			str = "keyword";
			break;
		case TokenType::LeftRoundBracket:
			str = "leftroundbracket";
			break;
		case TokenType::RightRoundBracket:
			str = "rightroundbracket";
			break;
		case TokenType::LeftSquareBracket:
			str = "leftsquarebracket";
			break;
		case TokenType::RightSquareBracket:
			str = "rightsquarebracket";
			break;
		case TokenType::LeftCurlyBracket:
			str = "leftcurlybracket";
			break;
		case TokenType::RightCurlyBracket:
			str = "rightcurlybracket";
			break;
		case TokenType::LessThan:
			str = "lessthan";
			break;
		case TokenType::GreaterThan:
			str = "greaterthan";
			break;
		case TokenType::Equal:
			str = "equal";
			break;
		case TokenType::Assignment:
			str = "assignment";
			break;
		case TokenType::Plus:
			str = "plus";
			break;
		case TokenType::Minus:
			str = "minus";
			break;
		case TokenType::Asterisk:
			str = "asterisk";
			break;
		case TokenType::Slash:
			str = "slash";
			break;
		case TokenType::Backslash:
			str = "backslash";
			break;
		case TokenType::Dot:
			str = "dot";
			break;
		case TokenType::Comma:
			str = "comma";
			break;
		case TokenType::Colon:
			str = "colon";
			break;
		case TokenType::Semicolon:
			str = "semicolon";
			break;
		case TokenType::End:
			str = "end";
			break;
		case TokenType::EndLine:
			str = "endline";
			break;
		case TokenType::Unexpected:
			str = "unexpected";
			break;
		default:
			throw "Error: invalid token type";
			break;
		}
		return str;
	}

	bool checkRow( Row row ) {
		if ( row.guidingSets.find( m_currentToken.value ) != row.guidingSets.end() ) {
			return true;
		}
		std::string tokenType = "[";
		tokenType += convertToString( m_currentToken.type );
		tokenType += "]";
		return row.guidingSets.find( tokenType ) != row.guidingSets.end();
	}

	void isShift( Row row ) {
		if ( row.shift ) {
			if ( m_currentToken.type != TokenType::End ) {
				m_currentToken = m_lexer.GetToken();
			}
		}
		return;
	};

	void isStack( Row row ) {
		if ( row.stack ) {
			m_stack.push( m_currentTableIndex + 1 );
		}
		return;
	}
};
