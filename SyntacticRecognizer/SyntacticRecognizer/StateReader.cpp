#include "StateReader.h"
#include <fstream>
#include <sstream>
#include <algorithm>


StateReader::StateReader( const std::string& path )
	:m_filePath( path )
{
}


StateReader::~StateReader()
{
}

std::vector<std::shared_ptr<State>> StateReader::GetStates()
{
	std::vector<std::shared_ptr<State>> states;
	std::ifstream file( m_filePath );
	if ( !file.is_open() )
	{
		throw "file not found";
	}
	states = ReadStates( file );
	ReadTransitions( states, file );
	return states;
}

std::shared_ptr<State> StateReader::GetStartState()
{
	std::vector<std::shared_ptr<State>> states = GetStates();
	return FindState( states, "Start" );
}

std::vector<std::shared_ptr<State>> StateReader::ReadStates( std::ifstream & stream )
{
	std::vector<std::shared_ptr<State>> states;
	std::string str;
	std::getline( stream, str );
	std::stringstream strStream( str );
	std::string stateName;
	while ( std::getline( strStream, stateName, ';' ) )
	{
		if ( stateName != "" )
		{
			std::shared_ptr<State> state = std::make_shared<State>( stateName );
			states.push_back( state );
		}
	}
	return states;
}

void StateReader::ReadTransitions( std::vector<std::shared_ptr<State>>& states, std::ifstream & stream )
{
	std::string str;
	while ( std::getline( stream, str ) )
	{
		std::string inputValueStr;
		std::stringstream streamStr( str );
		std::getline( streamStr, inputValueStr, ';' );
		char inputValueChar;
		if ( inputValueStr[0] == '\"' )
		{
			if ( inputValueStr.length() == 1 )
			{
				std::string temp;
				std::getline( streamStr, temp, ';' );
				inputValueChar = ';';
			}
			else
			{
					inputValueChar = inputValueStr[1];
			}
		}
		else
		{
			if ( inputValueStr == "\\n" )
			{
				inputValueChar = '\n';
			}
			else if ( inputValueStr == "\\t" )
			{
				inputValueChar = '\t';
			}
			else if ( inputValueStr == "\\0" )
			{
				inputValueChar = '\0';
			}
			else
			{
				inputValueChar = inputValueStr[0];
			}
		}
		int i = 0;
		std::string nameState;
		while ( std::getline( streamStr, nameState, ';' ) )
		{
			if ( nameState != "" )
			{
				std::shared_ptr<State> nextState = FindState( states, nameState );
				if ( nextState != NULL )
				{
					states[i]->AddTransition( inputValueChar, nextState );
				}
			}
			i++;
		}
	}
}

std::shared_ptr<State> StateReader::FindState( const std::vector<std::shared_ptr<State>>& states, std::string stateName )
{
	std::transform( stateName.begin(), stateName.end(), stateName.begin(), tolower );
	for ( std::shared_ptr<State> state : states )
	{
		std::string name = state->GetName();
		std::transform( name.begin(), name.end(), name.begin(), tolower );
		if ( name == stateName )
		{
			return state;
		}
	}
	return NULL;
}
