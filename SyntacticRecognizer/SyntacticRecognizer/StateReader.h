#pragma once
#include "State.h"
#include <string>
#include <vector>

class StateReader
{
public:
	StateReader( const std::string& path );
	~StateReader();
	std::vector<std::shared_ptr<State>> GetStates();
	std::shared_ptr<State> GetStartState();
private:
	std::string m_filePath;
	std::vector<std::shared_ptr<State>> ReadStates( std::ifstream& stream );
	void ReadTransitions( std::vector<std::shared_ptr<State>>& states, std::ifstream& stream );
	std::shared_ptr<State> FindState( const std::vector<std::shared_ptr<State>>& states, std::string stateName );
};

