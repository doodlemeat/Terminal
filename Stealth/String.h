#pragma once

#include <vector>
#include <string>

namespace sf
{
	class String;
}

namespace String
{
	std::vector<sf::String> explode(sf::String str, sf::String delim);
	std::vector<std::string> explode(std::string str, std::string delim);

	int to_int();
	float to_float();
}