#pragma once

#include <map>

namespace sf
{
	class String;
	class Font;
};

class Font
{
public:
	Font();
	~Font();

	static void cleanup();
	static void add(sf::String name, sf::String filename);
	static sf::Font* get(sf::String name);
	static void configure();

private:
	static std::map<sf::String, sf::Font*> fonts;
	static std::string m_path;
};

