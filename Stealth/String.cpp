#include "String.h"
#include "SFML\System\String.hpp"

namespace String
{
	std::vector<std::string> explode(std::string str, std::string delim)
	{
		std::string next = "";
		std::vector<std::string> result;
		result.clear();

		for (auto it = str.begin(); it != str.end(); it++)
		{
			if (std::string(1, *it) == delim)
			{
				if (!next.empty())
				{
					result.push_back(next);
					next.clear();
				}
			}
			else {
				next += *it;
			}
		}
		if (!next.empty())
			result.push_back(next);
		return result;
	}

	std::vector<sf::String> explode(sf::String str, sf::String delim)
	{
		sf::String next = "";
		std::vector<sf::String> result;
		result.clear();

		for (auto it = str.begin(); it != str.end(); it++)
		{
			if (*it == delim)
			{
				if (!next.isEmpty())
				{
					result.push_back(next);
					next.clear();
				}
			}
			else {
				next += *it;
			}
		}
		if (!next.isEmpty())
			result.push_back(next);
		return result;
	}
}