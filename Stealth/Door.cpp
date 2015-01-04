#include "Door.h"

#include <SFML/Graphics/Sprite.hpp>


Door::Door()
{
	m_open = false;
	m_sprite = new sf::Sprite();
}


Door::~Door()
{
}

sf::Sprite* Door::getSprite()
{
	return m_sprite;
}