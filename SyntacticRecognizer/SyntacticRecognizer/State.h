#pragma once
#include <memory>
#include <string>
#include <map>

class State
{
public:
	State( const std::string& name );
	~State();
	std::string GetName() const;
	std::shared_ptr<State> GetNextState( const char& inputValue );
	void AddTransition( const char& inputValue, std::shared_ptr<State> nextState );
private:
	std::string m_name;
	std::map<char, std::shared_ptr<State>> m_transitions;
};

