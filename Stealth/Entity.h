#pragma once

#include <vector>

#include "SFML\Graphics\Sprite.hpp"
#include "SFML\System\Time.hpp"

class Entity: public sf::Sprite
{
public:
	Entity();
	~Entity();

	void setVelocityX(float value);
	void setVelocityY(float value);
	void setVelocity(sf::Vector2f value);
	sf::Vector2f getVelocity();

	void move(sf::Time deltatime);
	void stop();

	void setSpeed(float value);
	float getSpeed();

	void rotateTo(sf::Vector2f value);
	void collideWith(std::vector<sf::FloatRect> bounds);

private:
	sf::Vector2f velocity;
	float speed;
};

