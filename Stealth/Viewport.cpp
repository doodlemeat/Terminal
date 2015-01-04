#include "Viewport.h"
#include "SFML\Graphics\View.hpp"
#include "SFML\Window\Mouse.hpp"

Viewport::Viewport(float width, float height)
{
	viewport = new sf::View();
	viewport->reset(sf::FloatRect(0.f, 0.f, width, height));
	viewport->setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
}


Viewport::~Viewport()
{
	delete viewport;
	viewport = nullptr;
}

void Viewport::setMax(sf::Vector2f value)
{
	max = value;
}

sf::View Viewport::get()
{
	return *viewport;
}
sf::View* Viewport::getView()
{
	return viewport;
}

void Viewport::update(const sf::Vector2f &handle)
{
	sf::Vector2f position;
	if (handle.x > max.x - viewport->getSize().x / 2)
		position.x = max.x - viewport->getSize().x / 2;
	else if (handle.x > viewport->getSize().x / 2)
		position.x = handle.x;
	else
		position.x = viewport->getSize().x / 2;
	
	if (handle.y > max.y - viewport->getSize().y / 2)
		position.y = max.y - viewport->getSize().y / 2;
	else if (handle.y > viewport->getSize().y / 2)
		position.y = handle.y;
	else
		position.y = viewport->getSize().y / 2;

	viewport->setCenter(position);
}