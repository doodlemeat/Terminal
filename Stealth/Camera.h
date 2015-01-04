#pragma once

#include "SFML\Graphics\Drawable.hpp"
#include "SFML\Graphics\Transformable.hpp"
#include "SFML\Graphics\Color.hpp"

namespace sf
{
	class Sprite;
	class Time;
}

class Light;
class Player;
class LightEngine;

class Camera: public sf::Drawable, public sf::Transformable {
public:
	Camera();
	~Camera();

	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
	void update(sf::Time deltatime);
	void createVision(LightEngine* lightEngine);

	sf::Sprite* getSprite();
	Light* getLight();
	Player* getPlayer();

	void setRotation(float start, float delta);
	void setRotationSpeed(float value);
	void setRotationDirection(bool p_value);
	void setPlayer(Player* player);
	float getAlertDistance();
	void setActive(bool p_value);
	bool isActive();
private:
	sf::Sprite* m_sprite;
	Light* m_light;
	sf::Color m_light_color;
	sf::Color m_light_color_detected;

	float m_rotation_speed;
	float m_current_rotation;
	float m_max_rotation;
	float m_min_rotation;
	bool m_shouldChangeDirection;
	Player* m_player;
	float m_alertDistance;
	bool active;
};