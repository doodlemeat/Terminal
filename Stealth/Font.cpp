#include "Font.h"
#include "SFML\System\String.hpp"
#include "SFML\Graphics\Font.hpp"
#include "Config.h"
#include <iostream>

std::map<sf::String, sf::Font*> Font::fonts;
std::string Font::m_path;

Font::Font()
{
	
}


Font::~Font()
{
}

void Font::cleanup()
{
	auto it = fonts.begin();
	while (it != fonts.end())
	{
		delete it->second;
		++it;
	}
}

void Font::add(sf::String name, sf::String filename)
{
	sf::Font* font = new sf::Font();
	auto it = fonts.find(name);

	if (it == fonts.end())
	{
		font->loadFromFile(m_path + filename);
		fonts.insert(std::pair<sf::String, sf::Font*>(name, font));
	}
}

sf::Font* Font::get(sf::String value)
{
	auto it = fonts.find(value);
	if (it != fonts.end())
	{
		return it->second;
	}
	return nullptr;
}
void Font::configure()
{
	m_path = Config::root_node["general"]["font_path"].as<std::string>();
	if (Config::root_node["fonts"])
	{
		for (YAML::iterator it = Config::root_node["fonts"].begin(); it != Config::root_node["fonts"].end(); ++it)
		{
			add(it->first.as<std::string>(), it->second.as<std::string>());
			std::cout << "Added font " << it->first.as<std::string>() << ", src: " << it->second.as<std::string>() << std::endl;
		}
	}
}