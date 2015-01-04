#pragma once

#include "SFML/Graphics/Rect.hpp"

class Goal
{
public:
	Goal();
	~Goal();

	void useKeycard(bool p_value);
	void setArea(float p_x, float p_y, float p_width, float p_height);

	sf::FloatRect* getArea();
private:
	bool m_useKeycard;
	sf::FloatRect m_area;
};

