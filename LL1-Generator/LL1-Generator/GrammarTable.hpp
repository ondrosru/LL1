#include <string>
#include <set>
#include <vector>
#include <iostream>

struct RowGrammarTable {
	std::string symbol;
	std::set<std::string> guidingSets;
	bool shift;
	std::string next;
	bool stack;
	bool error;
	bool endState;
};

class GrammarTable {
public:
	GrammarTable() {};
	~GrammarTable() {};
	
	void addRow( std::string symbol, std::set<std::string> guidingSets, bool shift, std::string next, bool stack, bool error, bool endState ) {
		m_rows.push_back( { symbol, guidingSets, shift, next, stack, error, endState } );
	};

	void changeRow( size_t id, std::string symbol, std::set<std::string> guidingSets, bool shift, std::string next, bool stack, bool error, bool endState ) {
		if ( id >= 0 && id < m_rows.size() ) {
			m_rows[id] = { symbol, guidingSets, shift, next, stack, error, endState };
		}
	};

	void changeRow( size_t id, RowGrammarTable rowTable ) {
		if ( id >= 0 && id < m_rows.size() ) {
			m_rows[id] = rowTable;
		}
	};

	size_t amountRow() {
		return m_rows.size();
	};

	RowGrammarTable getRow( size_t row ) {
		return m_rows[row];
	};

	void print( std::ostream& stream ) {
		stream << "# symbol guidingSets shift next stack error endState" << std::endl;
		for ( size_t i = 0; i < m_rows.size(); i++ ) {
			RowGrammarTable row = getRow( i );
			stream << i + 1 << " "
				<< row.symbol << " ";
			for ( std::string guidingSymbol : row.guidingSets ) {
				if ( !(*row.guidingSets.begin() == guidingSymbol) ) {
					stream << "|";
				}
				stream << guidingSymbol;
			}
			stream << " " << convertBoolToInt( row.shift ) << " "
				<< row.next << " "
				<< convertBoolToInt( row.stack ) << " "
				<< convertBoolToInt( row.error ) << " "
				<< convertBoolToInt( row.endState ) << std::endl;
		}
	};
private:
	std::vector<RowGrammarTable> m_rows;

	int convertBoolToInt( bool value ) {
		return value ? 1 : 0;
	};

	std::string convertBool( bool value ) {
		return value ? "yes" : "no";
	}
};
