#pragma once

#include "SFML\System\Vector2.hpp"

namespace sf
{
	class View;
};

class Viewport
{
public:
	Viewport(float width, float height);
	~Viewport();

	void setMax(sf::Vector2f value);
	sf::View get();
	sf::View* getView();

	void update(const sf::Vector2f &handle);

private:
	sf::View* viewport;
	sf::Vector2f max;
};

