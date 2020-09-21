#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>
#include <set>
#include "GrammarTable.hpp"

struct GrammarSymbol {
	std::string value;
	bool isTerminal = false;

	bool operator==( const GrammarSymbol& symbol ) const {
		return value == symbol.value && isTerminal == symbol.isTerminal;
	}
};

using RightRule = std::vector<GrammarSymbol>;
using GuidingSets = std::set<std::string>;

struct Rule {
	GrammarSymbol left;
	std::vector<RightRule> rightRules;
};

class Grammar {
public:
	Grammar() {};
	~Grammar() {};
	void readFile( std::string path ) {
		std::ifstream stream( path );
		std::string line;
		while ( std::getline( stream, line ) ) {
			std::stringstream lineStream( line );
			if ( lineStream.rdbuf()->in_avail() != 0 ) {
				Rule newRule;
				std::string value;
				lineStream >> value;
				newRule.left = convertToGrammarSymbol( value );
				RightRule rightRule;
				lineStream >> value; //skip "->"
				while ( lineStream.rdbuf()->in_avail() != 0 ) {

					lineStream >> value;
					if ( value == "|" ) {
						newRule.rightRules.push_back( rightRule );
						rightRule.clear();
					}
					else {
						rightRule.push_back( convertToGrammarSymbol( value ) );
					}
				}
				newRule.rightRules.push_back( rightRule );
				m_rules.push_back( newRule );
			}
		}
		m_axiom = m_rules[0].left;
		removeLeftRecursions();
	};

	void write( std::ostream& out ) const {
		for ( std::vector<Rule>::const_iterator it = m_rules.begin(); it != m_rules.end(); it++ ) {
			out << it->left.value << " ->";
			for ( size_t i = 0; i < it->rightRules.size(); i++ ) {
				for ( RightRule::const_iterator symbolIt = it->rightRules[i].begin(); symbolIt != it->rightRules[i].end(); symbolIt++ ) {
					out << " " << symbolIt->value << " ";
				}
				if ( (i + 1) < it->rightRules.size() ) {
					out << "|";
				}
			}
			out << std::endl;
		}
	};

	GrammarTable toTableLL1() {
		std::map < std::string, std::vector<GuidingSets> > guidingSets = getGuidingSets();
		std::map<std::string, size_t> positionsNonTerminal;
		size_t index = 1;
		for ( Rule rule : m_rules ) {
			positionsNonTerminal.insert( { rule.left.value , index } );
			for ( RightRule rightRule : rule.rightRules ) {
				index++;
			}

			for ( RightRule rightRule : rule.rightRules ) {
				for ( size_t i = 0; i < rightRule.size(); i++ ) {
					index++;
				}
			}
		}

		GrammarTable table;
		for ( Rule rule : m_rules ) {
			for ( size_t i = 0; i < rule.rightRules.size(); i++) {
				size_t pos = table.amountRow() + rule.rightRules.size() - i + 1;
				for ( size_t j = 0; j < i; j++ ) {
					pos += rule.rightRules[j].size();
				}
				bool error = true;
				if ( i + 1 < rule.rightRules.size() ) {
					error = false;
				}
				table.addRow( rule.left.value, guidingSets[rule.left.value][i], false, std::to_string( pos ),  false, error, false);
			}
			for ( size_t j = 0; j < rule.rightRules.size(); j++ ) {
				for ( size_t i = 0; i < rule.rightRules[j].size(); i++ ) {
					std::set<std::string> tempGuidingSets;
					bool shift = false;
					bool stack = false;
					bool endState = false;
					std::string nextPosition = "-";
					if ( rule.rightRules[j][i].isTerminal ) {
						if ( rule.rightRules[j][i].value != EMPTY_SYMBOL ) {
							tempGuidingSets.insert( rule.rightRules[j][i].value );
						}
						else {
							for ( std::string value : guidingSets[rule.left.value][j] ) {
								tempGuidingSets.insert( value );
							}
						}
						shift = true;
						if ( i + 1 < rule.rightRules[j].size() ) {
							nextPosition = std::to_string( table.amountRow() + 2 );
						}
					}
					else {
						if ( i + 1 < rule.rightRules[j].size() ) {
							stack = true;;
						}
						for ( std::set<std::string> values : guidingSets[rule.rightRules[j][i].value] ) {
							for ( std::string value : values ) {
								tempGuidingSets.insert( value );
							}
						}
						nextPosition = std::to_string( positionsNonTerminal[rule.rightRules[j][i].value] );
					}
					if ( i + 1 >= rule.rightRules[j].size() && rule.left == m_axiom ) {
						endState = true;
						shift = false;
					}
					if ( rule.rightRules[j][i].value == EMPTY_SYMBOL ) {
						shift = false;
					}
					table.addRow( rule.rightRules[j][i].value, tempGuidingSets, shift, nextPosition, stack, true, endState );
				}
			}
		}
		return table;
	};
private:
	std::vector<Rule> m_rules;
	const std::string EMPTY_SYMBOL = "{empty}";
	const std::string END_SYMBOL = "[end]";
	GrammarSymbol m_axiom;

	bool isTerminal( std::string value ) {
		return !(value[0] == '<' && value[value.size() - 1] == '>');
	};

	GrammarSymbol convertToGrammarSymbol( std::string value ) {
		GrammarSymbol symbol;
		symbol.value = value;
		symbol.isTerminal = isTerminal( value );
		return symbol;
	};

	void removeLeftRecursions() {
		std::vector<Rule> newRules;
		for ( std::vector<Rule>::iterator it = m_rules.begin(); it != m_rules.end(); it++ ) {
			bool isLeftRecursion = false;
			for ( size_t i = 0; i < it->rightRules.size(); i++ ) {
				if ( it->rightRules[i][0].value == it->left.value ) {
					isLeftRecursion = true;
					break;
				}
			}

			if ( isLeftRecursion ) {
				GrammarSymbol newSymbol;
				newSymbol.isTerminal = false;
				newSymbol.value = "<";
				newSymbol.value += removeBothChar( it->left.value );
				newSymbol.value += "_copy>";
				Rule rule1;
				Rule rule2;
				rule1.left = it->left;
				rule2.left = newSymbol;
				RightRule emptyRightRule;
				emptyRightRule.push_back( { EMPTY_SYMBOL, true } );
				rule2.rightRules.push_back( emptyRightRule );
				for ( size_t i = 0; i < it->rightRules.size(); i++ ) {
					if ( it->rightRules[i][0].value == it->left.value ) {
						RightRule newRightRule = it->rightRules[i];
						newRightRule.push_back( newSymbol );
						newRightRule.erase( newRightRule.begin() );
						rule2.rightRules.push_back( newRightRule );
					}
					else {
						RightRule newRightRule;
						if ( it->rightRules[i][0].value != EMPTY_SYMBOL ) {
							newRightRule = it->rightRules[i];
						}
						newRightRule.push_back( newSymbol );
						rule1.rightRules.push_back( newRightRule );
					}
				}
				it->rightRules = rule1.rightRules;
				newRules.push_back( rule2 );
			}
		}
		for ( std::vector<Rule>::iterator it = newRules.begin(); it != newRules.end(); it++ ) {
			m_rules.push_back( *it );
		}
	};

	std::map < std::string, std::vector<GuidingSets> > getGuidingSets() {
		std::map < std::string, std::vector<GuidingSets> > rulesGuidingSets;
		for ( Rule rule : m_rules ) {
			std::vector<GuidingSets> guidingSets;
			std::queue<std::pair<size_t, GrammarSymbol>> queueFirst;
			std::set<std::string> firstAdded;
			std::queue<std::pair<size_t, GrammarSymbol>> queueFollow;
			std::set<std::string> followAdded;
			std::vector<GrammarSymbol> firsts = getFirst( rule.left );
			for ( size_t i = 0; i < firsts.size(); i++ ) {
				std::set<std::string> emptySetSymbol;
				guidingSets.push_back( emptySetSymbol );
				if ( firsts[i].value == EMPTY_SYMBOL ) {
					if ( followAdded.find( rule.left.value ) == followAdded.end() ) {
						queueFollow.push( { i, rule.left } );
						followAdded.insert( rule.left.value );
					}
				}
				else if ( firsts[i].isTerminal ) {
					guidingSets[i].insert( firsts[i].value );
				}
				else {
					if ( firstAdded.find( firsts[i].value ) == firstAdded.end() ) {
						queueFirst.push( { i, firsts[i] } );
						firstAdded.insert( firsts[i].value );
					}
				}
			}

			while ( !queueFirst.empty() || !queueFollow.empty() ) {
				while ( !queueFirst.empty() ) {
					std::pair<size_t, GrammarSymbol> symbol = queueFirst.front();
					queueFirst.pop();
					std::vector<GrammarSymbol> newFirst = getFirst( symbol.second );
					for ( GrammarSymbol firstSymbol : newFirst ) {
						if ( firstSymbol.value == EMPTY_SYMBOL ) {
							if ( followAdded.find( firstSymbol.value ) == followAdded.end() ) {
								queueFollow.push( { symbol.first, firstSymbol } );
								followAdded.insert( firstSymbol.value );
							}
						}
						else if ( firstSymbol.isTerminal ) {
							guidingSets[symbol.first].insert( firstSymbol.value );
						}
						else {
							if ( firstAdded.find( firstSymbol.value ) == firstAdded.end() ) {
								queueFirst.push( { symbol.first, firstSymbol } );
								firstAdded.insert( firstSymbol.value );
							}
						}
					}
				}
				while ( !queueFollow.empty() ) {
					std::pair<size_t, GrammarSymbol> symbol = queueFollow.front();
					queueFollow.pop();
					std::vector<std::pair<bool, GrammarSymbol>> newFollow = getFollow( symbol.second );
					for ( std::pair<bool, GrammarSymbol> followSymbol : newFollow ) {
						if ( followSymbol.second.isTerminal ) {
							guidingSets[symbol.first].insert( followSymbol.second.value );
						}
						else {
							if ( followSymbol.first ) {
								if ( followAdded.find( followSymbol.second.value ) == followAdded.end() ) {
									queueFollow.push( { symbol.first, followSymbol.second } );
									followAdded.insert( followSymbol.second.value );
								}
							}
							else {
								if ( firstAdded.find( followSymbol.second.value ) == firstAdded.end() ) {
									queueFirst.push( { symbol.first, followSymbol.second } );
									firstAdded.insert( followSymbol.second.value );
								}
							}
						}
					}
				}
			}

			rulesGuidingSets.insert( { rule.left.value, guidingSets } );
		}
		return rulesGuidingSets;
	};

	std::string removeBothChar( std::string value ) {
		return value.substr( 1, value.size() - 2 );
	};

	std::vector<GrammarSymbol> getFirst( GrammarSymbol symbol ) {
		std::vector<GrammarSymbol> firsts;
		for ( Rule rule : m_rules ) {
			if ( rule.left == symbol ) {
				for ( RightRule rightRule : rule.rightRules ) {
					firsts.push_back( rightRule[0] );
				}
			}
		}
		return firsts;
	};

	std::vector<std::pair<bool, GrammarSymbol>> getFollow( GrammarSymbol symbol ) {
		std::vector<std::pair<bool, GrammarSymbol>> follows;
		for ( Rule rule : m_rules ) {
			for ( RightRule rightRule : rule.rightRules ) {
				for ( size_t i = 0; i < rightRule.size(); i++ ) {
					if ( rightRule[i] == symbol ) {
						if ( i + 1 >= rightRule.size() ) {
							if ( symbol == m_axiom ) {
								GrammarSymbol newSymbol;
								newSymbol.value = END_SYMBOL;
								newSymbol.isTerminal = true;
								follows.push_back( { true, newSymbol } );
							}
							else {
								follows.push_back( { true, rule.left } );
							}
						}
						else {
							follows.push_back( { false, rightRule[i + 1] } );
						}
					}
				}
			}
		}
		return follows;
	};
};