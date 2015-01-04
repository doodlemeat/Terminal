#include "Entity.h"




Entity::Entity()
{
	velocity = sf::Vector2f(0.f, 0.f);
	speed = 0.f;
}


Entity::~Entity()
{
}

void Entity::setVelocityX(float value)
{
	velocity.x = value;
}
void Entity::setVelocityY(float value)
{
	velocity.y = value;
}
void Entity::setVelocity(sf::Vector2f value)
{
	velocity = value;
}

sf::Vector2f Entity::getVelocity()
{
	return velocity;
}

void Entity::move(sf::Time deltatime)
{
	setPosition(sf::Vector2f(getPosition().x + deltatime.asSeconds() * velocity.x, getPosition().y + deltatime.asSeconds() * velocity.y));
}

void Entity::stop()
{
	velocity.x = 0.f;
	velocity.y = 0.f;
}

void Entity::setSpeed(float value)
{
	speed = value;
}
float Entity::getSpeed()
{
	return speed;
}

void Entity::rotateTo(sf::Vector2f value)
{
	float radians = atan2f(-getPosition().y - -value.y, -getPosition().x - -value.x);
	setRotation(radians * (180.f / 3.14f));
}

void Entity::collideWith(std::vector<sf::FloatRect> bounds)
{
	for (auto &rect : bounds)
	{
		float length_x = fabs(getPosition().x - (rect.left + rect.width * 0.5));
		float length_y = fabs(getPosition().y - (rect.top + rect.height * 0.5));

		if (length_x < (this->getLocalBounds().width * 0.5 + rect.width * 0.5) && length_y < (this->getLocalBounds().height * 0.5 + rect.height * 0.5))
		{
			float overflow_x = length_x - (this->getLocalBounds().width * 0.5 + rect.width * 0.5);
			float overflow_y = length_y - (this->getLocalBounds().height * 0.5 + rect.height * 0.5);

			if (overflow_x > overflow_y)
			{
				if (this->getPosition().x >rect.left)
					this->setPosition(sf::Vector2f(this->getPosition().x - overflow_x, this->getPosition().y));
				else
					this->setPosition(sf::Vector2f(this->getPosition().x + overflow_x, this->getPosition().y));
			}
			else
			{
				if (this->getPosition().y >rect.top)
					this->setPosition(sf::Vector2f(this->getPosition().x, this->getPosition().y - overflow_y));
				else
					this->setPosition(sf::Vector2f(this->getPosition().x, this->getPosition().y + overflow_y));
			}
		}
	}
}
