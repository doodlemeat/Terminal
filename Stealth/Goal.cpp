#include "Goal.h"


Goal::Goal()
{
}


Goal::~Goal()
{
}


void Goal::useKeycard(bool p_value)
{
	m_useKeycard = p_value;
}

void Goal::setArea(float p_x, float p_y, float p_width, float p_height)
{
	m_area.left = p_x;
	m_area.height = p_height;
	m_area.top = p_y;
	m_area.width = p_width;
}

sf::FloatRect* Goal::getArea()
{
	return &m_area;
}