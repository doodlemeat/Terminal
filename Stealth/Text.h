#pragma once

#include <map>
#include "SFML\Graphics\Text.hpp"

namespace sf
{
	class String;
};

class TextObject: public sf::Text
{
	using sf::Text::setFont;
public:
	TextObject();
	void setFont(sf::String name);
};

class Text {
public:
	Text();
	~Text();

	static void cleanup();
	static void add(sf::String name);
	static TextObject* get(sf::String name);

private:
	static std::map<sf::String, TextObject*> texts;
};

