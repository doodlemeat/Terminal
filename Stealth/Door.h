#pragma once

#include "SFML/Graphics/Rect.hpp"

#include <vector>

namespace sf
{
	class Sprite;
}

class Door
{
public:
	Door();
	~Door();

	sf::Sprite* getSprite();

private:
	sf::Sprite* m_sprite;
	bool m_open;
	sf::FloatRect m_collisionBounds;
};

