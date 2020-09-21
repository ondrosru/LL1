#pragma once
#include "TokenType.h"
struct Token
{
	std::string value;
	TokenType type;
	size_t row;
	size_t col;
};