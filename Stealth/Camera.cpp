#include "Camera.h"
#include "Config.h"
#include "LightEngine.h"
#include "SFML\Graphics\Color.hpp"
#include "SFML\System\Time.hpp"
#include "Player.h"
#include "Audiomanager.h"
#include <iostream>
#include "SFML\Audio\Music.hpp"

Camera::Camera()
{
	if (Config::root_node["cameras"])
	{
		m_light_color = sf::Color(
			Config::root_node["cameras"]["vision_r"].as<int>(),
			Config::root_node["cameras"]["vision_g"].as<int>(),
			Config::root_node["cameras"]["vision_b"].as<int>(),
			Config::root_node["cameras"]["vision_a"].as<int>()
		);
		m_light_color_detected = sf::Color(
			Config::root_node["cameras"]["vision_r_detected"].as<int>(),
			Config::root_node["cameras"]["vision_g_detected"].as<int>(),
			Config::root_node["cameras"]["vision_b_detected"].as<int>(),
			Config::root_node["cameras"]["vision_a_detected"].as<int>()
		);
		m_alertDistance = Config::root_node["cameras"]["alert_distance"].as<float>();
	}

	m_shouldChangeDirection = false;
	m_sprite = new sf::Sprite();
}
Camera::~Camera()
{
	delete m_sprite;
	m_sprite = nullptr;
}

void Camera::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	rt.draw(*m_sprite);
}
void Camera::update(sf::Time deltatime)
{
	if (m_shouldChangeDirection)
	{
		m_current_rotation += m_rotation_speed * deltatime.asSeconds();
		if (m_current_rotation >= m_max_rotation) m_shouldChangeDirection = !m_shouldChangeDirection;
	}
	else
	{
		m_current_rotation -= m_rotation_speed * deltatime.asSeconds();
		if (m_current_rotation <= m_min_rotation) m_shouldChangeDirection = !m_shouldChangeDirection;
	}
	m_sprite->setRotation(m_current_rotation);
	m_light->update(getSprite()->getPosition(), getSprite()->getRotation());
}

sf::Sprite* Camera::getSprite()
{
	return m_sprite;
}
void Camera::createVision(LightEngine* lightEngine)
{
	std::vector<sf::Vector2f> lightPoly;
	lightPoly.emplace_back(0.f, 0.f);
	lightPoly.emplace_back(100.f, -50.f);
	lightPoly.emplace_back(120.f, 0.f);
	lightPoly.emplace_back(100.f, 50.f);
	m_light = lightEngine->addLight(getSprite()->getPosition(), lightPoly, 0);
	m_light->m_color = m_light_color;
	m_light->m_color_detected = m_light_color_detected;
	m_light->m_current_color = m_light_color;
}
Light* Camera::getLight()
{
	return m_light;
}
void Camera::setRotation(float start, float delta)
{
	// This method must be called before setting the rotation of the sprite
	m_sprite->setRotation(start);
	m_current_rotation = start;
	m_max_rotation = m_current_rotation + delta;
	m_min_rotation = m_current_rotation - delta;
}
void Camera::setRotationSpeed(float value)
{
	m_rotation_speed = value;
}
void Camera::setPlayer(Player* player)
{
	m_player = player;
}
Player* Camera::getPlayer()
{
	return m_player;
}

float Camera::getAlertDistance()
{
	return m_alertDistance;
}

void Camera::setActive(bool p_value)
{
	active = p_value;
}
bool Camera::isActive()
{
	return (active);
}

void Camera::setRotationDirection(bool p_value)
{
	m_shouldChangeDirection = p_value;
}
