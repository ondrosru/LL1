#include <string>
#include <sstream>
#include <set>
#include <map>
#include <fstream>

struct GrammaSymbol {
	std::string name;
	bool isTerminal;
};

struct Row {
	GrammaSymbol symbol;
	std::set<std::string> guidingSets;
	bool shift;
	int next;
	bool stack;
	bool error;
	bool endState;
};

class SyntaxTable {
public:
	SyntaxTable( std::string fileName ) {
		std::ifstream file( fileName );
		std::string line;
		std::getline( file, line ); //skip line
		while ( std::getline( file, line ) ) {
			std::stringstream lineStream( line );
			int index;
			std::string symbol;
			std::string guidingSetsStr;
			std::string shift;
			std::string next;
			std::string stack;
			std::string error;
			std::string endState;
			lineStream >> index;
			lineStream >> symbol;
			lineStream >> guidingSetsStr;
			lineStream >> shift;
			lineStream >> next;
			lineStream >> stack;
			lineStream >> error;
			lineStream >> endState;
			std::stringstream guidingSetsStream( guidingSetsStr );
			std::string guidingSet;
			std::set<std::string> guidingSets;
			while ( std::getline( guidingSetsStream, guidingSet, '|' ) ) {
				guidingSets.insert( guidingSet );
			}
			m_rows[index] = Row( { convertToSymbol( symbol ), guidingSets, convetToBool( shift ), nextStringToInt( next ), convetToBool( stack ), convetToBool( error ), convetToBool( endState ) } );
		}
	};
	~SyntaxTable() {};

	bool containsRow( int rowNum ) {
		return m_rows.find( rowNum ) != m_rows.end();
	};

	Row getRow( int rowNum ) {
		if ( !containsRow( rowNum ) ) {
			throw "Error: not found row.";
		}
		return m_rows[rowNum];
	}


private:
	std::map<int, Row> m_rows;

	GrammaSymbol convertToSymbol( std::string value ) {
		GrammaSymbol symbol;
		if ( value[0] == '<' && value[value.size() - 1] == '>' ) {
			symbol.isTerminal = false;
			symbol.name = value.substr( 1, value.size() - 2 );
		}
		else {
			symbol.isTerminal = true;
			symbol.name = value;
		}
		return symbol;
	}

	bool convetToBool( std::string value ) {
		if ( value == "1" ) {
			return true;
		}
		return false;
	}

	int nextStringToInt( std::string value ) {
		if ( value == "-" ) {
			return -1;
		}
		return atoi( value.c_str() );
	}
};
