#include "Text.h"
#include "Font.h"
#include "SFML\Graphics\Text.hpp"

std::map<sf::String, TextObject*> Text::texts;

Text::Text()
{
}


Text::~Text()
{
}


void Text::cleanup()
{
	auto it = texts.begin();
	while (it != texts.end())
	{
		delete it->second;
		it = texts.erase(it);
	}
}

void Text::add(sf::String name)
{
	TextObject* text = new TextObject();
	texts.insert(std::pair<sf::String, TextObject*>(name, text));
}

TextObject* Text::get(sf::String name)
{
	auto it = texts.find(name);
	if (it != texts.end())
	{
		return it->second;
	}
	return nullptr;
}

TextObject::TextObject() : sf::Text()
{

}

void TextObject::setFont(sf::String name)
{
	if (Font::get(name) == nullptr) {
		printf("Text::setFont - Font is nullptr\n");
	}
	setFont(*Font::get(name));
}