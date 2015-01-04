#include "Coin.h"
#include "SFML\System\Time.hpp"
#include "Item.h"
#include "SFML/Graphics/Sprite.hpp"
#include <iostream>
#include "Audiomanager.h"
#include "SoundrippleManager.h"


Coin::Coin()
{
	m_velocity.x = 0.f;
	m_velocity.y = 0.f;
}


Coin::~Coin()
{
}

void Coin::setVelocityX(float p_value)
{
	m_velocity.x = p_value;
}

void Coin::setVelocityY(float p_value)
{
	m_velocity.y = p_value;
}

void Coin::update(std::vector<sf::FloatRect> &collision_bounds, sf::Time deltatime)
{
	if (isMoving())
	{
		move();
		for (auto &rect: collision_bounds)
		{
			if (rect.intersects(getGroundSprite()->getGlobalBounds()))
			{
				Audiomanager::sfx("coin")->play();
				m_soundRippleManager->createRipple(getGroundSprite()->getPosition(), m_ripple_radius, RIPPLE_UNKNOWN);
				stop();
			}
		}
		sf::Vector2f currentPosition = getGroundSprite()->getPosition();
		if (fabs(currentPosition.x - m_destination.x) < 10.f && fabs(currentPosition.y - m_destination.y) < 10.f)
		{
			Audiomanager::sfx("coin")->play();
			m_soundRippleManager->createRipple(getGroundSprite()->getPosition(), m_ripple_radius, RIPPLE_UNKNOWN);
			stop();
		}
	}
}

void Coin::move()
{
	sf::Vector2f move_offset(0.f, 0.f);
	move_offset.x += m_velocity.x;
	move_offset.y += m_velocity.y;
	getGroundSprite()->move(move_offset);
}

bool Coin::isMoving()
{
	return (m_velocity.x != 0.f || m_velocity.y != 0.f);
}

void Coin::stop()
{
	m_velocity.x = 0.f;
	m_velocity.y = 0.f;
}

void Coin::setDestination(sf::Vector2f p_destination)
{
	m_destination = p_destination;
}

void Coin::setSoundRippleManager(SoundrippleManager* p_soundRippleManager)
{
	m_soundRippleManager = p_soundRippleManager;
}

void Coin::setSoundRippleRadius(float p_radius)
{
	m_ripple_radius = p_radius;
}
