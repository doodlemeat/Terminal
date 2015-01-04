#pragma once

#include "Animation.h"
#include "SFML\System\Vector2.hpp"
#include "SFML\Graphics\RectangleShape.hpp"

class GObject
{
public:
	GObject();
	~GObject();

	void addAnimation(Animation* animation);

	void setVelocityX(float value);
	void setVelocityY(float value);
	void setVelocity(sf::Vector2f value);
	void setSpeed(float value);
	void setCollidable(bool value);
	void setRotationRad(float value);
	void setRotation(float value);
	void setHitBox(std::string root_node);
	void drawHitbox();
	void updateHitbox();
	sf::Vector2f getVelocity();
	float getSpeed();
	float getRotationRad();
	float getRotation();
	AnimatedSprite* getAnimatedSprite();

	void rotate();
	void rotateRad();
	void move(sf::Time deltatime);
	virtual bool moving();
	void stop();
	void rotateTo(sf::Vector2f value);
	float getAngleTo(sf::Vector2f value);
	void collideWith(std::vector<sf::FloatRect> bounds);

	AnimatedSprite* animatedSprite;
	float rotation;
	bool isCollideable;
	sf::Vector2f velocity;
	float speed;
	sf::Vector2f hitbox_start;
	sf::Vector2f hitbox_extension;
	sf::RectangleShape hitbox;
};