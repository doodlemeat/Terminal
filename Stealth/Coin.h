#pragma once

#include "Item.h"
#include "SFML\System\Vector2.hpp"
#include <vector>
#include "SFML\Graphics\Rect.hpp"
#include "SFML\System\Time.hpp"

class SoundrippleManager;

class Coin: public Item
{
public:
	Coin();
	~Coin();
	
	void setVelocityX(float p_value);
	void setVelocityY(float p_value);
	void update(std::vector<sf::FloatRect> &collision_bounds, sf::Time deltatime);
	bool isMoving();
	void move();
	void stop();
	void setDestination(sf::Vector2f p_destination);
	void setSoundRippleManager(SoundrippleManager* p_soundRippleManager);
	void setSoundRippleRadius(float p_radius);
private:
	sf::Vector2f m_velocity;
	sf::Vector2f m_destination;
	SoundrippleManager* m_soundRippleManager;
	float m_ripple_radius;
};

