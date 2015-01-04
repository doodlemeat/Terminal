#pragma once
#include "Item.h"
#include "SFML/System/Clock.hpp"

class Disg: public Item
{
public:
	Disg();
	~Disg();

	sf::Clock* getTimer();

private:
	sf::Clock m_timer;
};

