#include "State.h"



State::State( const std::string& name )
	:m_name( name )
{
}


State::~State()
{
}

std::string State::GetName() const
{
	return m_name;
}

std::shared_ptr<State> State::GetNextState( const char& inputValue )
{
	auto nextState = m_transitions.find( inputValue );
	if ( nextState != m_transitions.end() )
	{
		return nextState->second;
	}
	return NULL;
}

void State::AddTransition( const char & inputValue, std::shared_ptr<State> nextState )
{
	std::pair<char, std::shared_ptr<State>> newTransition = std::make_pair( inputValue, nextState );
	m_transitions.insert( newTransition );
}
