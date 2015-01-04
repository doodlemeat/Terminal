#include "GObject.h"
#include "Angle.h"
#include "SFML\Graphics\RectangleShape.hpp"
#include "Window.h"
#include "Config.h"


GObject::GObject()
{
	velocity = sf::Vector2f(0.f, 0.f);
	speed = 0.f;
	animatedSprite = new AnimatedSprite();
}
GObject::~GObject()
{
	
}
void GObject::addAnimation(Animation* animation)
{
	animatedSprite->addAnimation(animation);
}
void GObject::setVelocityX(float value)
{
	velocity.x = value;
}
void GObject::setVelocityY(float value)
{
	velocity.y = value;
}
void GObject::setVelocity(sf::Vector2f value)
{
	velocity = value;
}
void GObject::setSpeed(float value)
{
	speed = value;
}
void GObject::setCollidable(bool value)
{
	this->isCollideable = value;
}
void GObject::setRotationRad(float value)
{
	this->rotation = value * Angle::RAD2DEG;
}
void GObject::setRotation(float value)
{
	this->rotation = value;
}
sf::Vector2f GObject::getVelocity()
{
	return velocity;
}
float GObject::getSpeed()
{
	return speed;
}
float GObject::getRotationRad()
{
	return rotation * Angle::DEG2RAD;
}
float GObject::getRotation()
{
	return rotation;
}
AnimatedSprite* GObject::getAnimatedSprite()
{
	return animatedSprite;
}
void GObject::move(sf::Time deltatime)
{
	sf::Vector2f old_position = hitbox.getPosition();
	hitbox.setPosition(
		old_position.x + deltatime.asSeconds() * velocity.x,
		old_position.y + deltatime.asSeconds() * velocity.y
	);
}
bool GObject::moving()
{
	return (velocity.x != 0.f || velocity.y != 0.f);
}
void GObject::stop()
{
	/*velocity.x *= 0.8f;
	velocity.y *= 0.8f;
	if (abs(velocity.x) < 0.1 && abs(velocity.y) < 0.1)
	{
		velocity.x = 0.f;
		velocity.y = 0.f;
	}*/
	velocity.x = 0.f;
	velocity.y = 0.f;
}
void GObject::rotateTo(sf::Vector2f value)
{
	float radians = atan2f(-animatedSprite->getPosition().y - -value.y, -animatedSprite->getPosition().x - -value.x);
	animatedSprite->setRotation(radians * (180.f / 3.14f));
}
float GObject::getAngleTo(sf::Vector2f value)
{
	return atan2f(-animatedSprite->getPosition().y - -value.y, -animatedSprite->getPosition().x - -value.x);
}
void GObject::collideWith(std::vector<sf::FloatRect> bounds)
{
	for (auto &rect : bounds)
	{
		sf::Vector2f hitbox_size = hitbox.getSize();
		sf::Vector2f hitbox_position = hitbox.getPosition();
		float length_x = fabs(hitbox_position.x - (rect.left + rect.width * 0.5f));
		float length_y = fabs(hitbox_position.y - (rect.top + rect.height * 0.5f));

		if (length_x < (hitbox_size.x * 0.5f + rect.width * 0.5f) && length_y < (hitbox_size.y * 0.5 + rect.height * 0.5))
		{
			float overflow_x = length_x - (hitbox_size.x * 0.5f + rect.width * 0.5f);
			float overflow_y = length_y - (hitbox_size.y * 0.5f + rect.height * 0.5f);

			if (overflow_x > overflow_y)
			{
				if (hitbox_position.x > rect.left + rect.width / 2)
					hitbox.setPosition(hitbox_position.x - overflow_x, hitbox_position.y);
				else
					hitbox.setPosition(hitbox_position.x + overflow_x, hitbox_position.y);
			}
			else
			{
				if (hitbox_position.y > rect.top + rect.height / 2)
					hitbox.setPosition(hitbox_position.x, hitbox_position.y - overflow_y);
				else
					hitbox.setPosition(hitbox_position.x, hitbox_position.y + overflow_y);
			}
		}
	}
}


void GObject::setHitBox(std::string root_node)
{
	if (Config::root_node[root_node])
	{
		this->hitbox_start.x = Config::root_node[root_node]["hitbox_x"].as<float>();
		this->hitbox_start.y = Config::root_node[root_node]["hitbox_y"].as<float>();
		this->hitbox_extension.x = Config::root_node[root_node]["hitbox_w"].as<float>();
		this->hitbox_extension.y = Config::root_node[root_node]["hitbox_h"].as<float>();
	}
	this->hitbox.setPosition(sf::Vector2f(getAnimatedSprite()->getPosition().x + this->hitbox_start.x, getAnimatedSprite()->getPosition().y + this->hitbox_start.y));
	this->hitbox.setSize(this->hitbox_extension);
	this->hitbox.setOrigin(this->hitbox.getSize().x / 2, this->hitbox.getSize().y / 2);
	this->hitbox.setOutlineColor(sf::Color::Red);
	this->hitbox.setOutlineThickness(2);
	this->hitbox.setFillColor(sf::Color::Transparent);
}
void GObject::drawHitbox()
{
	Window::get("main")->draw(this->hitbox);
}
void GObject::updateHitbox()
{
	this->getAnimatedSprite()->setPosition(sf::Vector2f(
		this->hitbox.getPosition().x, 
		this->hitbox.getPosition().y));
}