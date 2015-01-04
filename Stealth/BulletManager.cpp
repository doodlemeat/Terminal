#include "BulletManager.h"
#include "SpriteManager.h"
#include "Config.h"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include "Math.h"
#include "Guard.h"
#include "Player.h"
#include <iostream>
#include "Angle.h"

Bullet::Bullet(sf::Vector2f position, float angle)
{
	sf::Vector2f direction;
	direction.x = Math::cos(angle * Angle::DEG2RAD);
	direction.y = Math::sin(angle * Angle::DEG2RAD);
	this->direction = direction;
	this->setPosition(position);
	m_die = false;
}
Bullet::~Bullet()
{

}
void Bullet::update(sf::Time deltatime)
{
	sf::Vector2f position;
	sf::Vector2f last_pos = this->getPosition();
	position.x = last_pos.x + direction.x * speed;
	position.y = last_pos.y + direction.y * speed;
	this->setPosition(position);
}

BulletManager::BulletManager()
{
	if (Config::root_node["general"])
	{
		m_bullet_radius = 4;
		m_bullet_color_a = 255;
		m_bullet_color_r = 255;
		m_bullet_color_g = 255;
		m_bullet_color_b = 255;
		if (Config::root_node["general"]["bullet_radius"])
		{
			m_bullet_radius = Config::root_node["general"]["bullet_radius"].as<float>();
		}
		if (Config::root_node["general"]["bullet_color_r"])
		{
			m_bullet_color_r = Config::root_node["general"]["bullet_color_r"].as<int>();
		}
		if (Config::root_node["general"]["bullet_color_g"])
		{
			m_bullet_color_g = Config::root_node["general"]["bullet_color_g"].as<int>();
		}
		if (Config::root_node["general"]["bullet_color_b"])
		{
			m_bullet_color_b = Config::root_node["general"]["bullet_color_b"].as<int>();
		}
		if (Config::root_node["general"]["bullet_color_a"])
		{
			m_bullet_color_a = Config::root_node["general"]["bullet_color_a"].as<int>();
		}
		m_bulletSpeed = Config::root_node["general"]["bullet_speed"].as<float>();
	}
}
BulletManager::~BulletManager()
{
}
void BulletManager::update(Player* player, std::vector<Guard*> &guards, std::vector<sf::FloatRect> collision_bounds, sf::Time deltatime)
{
	for (unsigned int i = 0; i < m_bullets.size(); i++)
	{
		for (auto &rect : collision_bounds)
		{
			if (rect.intersects(m_bullets[i]->getGlobalBounds()))
			{
				m_bullets[i]->setDie(true);
			}
		}
		for (auto &guard : guards)
		{
			if (!guard->isDead() && guard->hitbox.getGlobalBounds().intersects(m_bullets[i]->getGlobalBounds()))
			{
				m_bullets[i]->setDie(true);
				guard->changeHP(-m_bullets[i]->getDamage());
			}
		}
		if (player->hitbox.getGlobalBounds().intersects(m_bullets[i]->getGlobalBounds()))
		{
			m_bullets[i]->setDie(true);
			player->changeHP(-m_bullets[i]->getDamage());
		}
		m_bullets[i]->update(deltatime);
	}
	for (unsigned int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i]->getDie())
		{
			delete m_bullets[i];
			m_bullets.erase(m_bullets.begin() + i);
		}
	}
}
void BulletManager::spawnBullet(sf::Vector2f position, float radians)
{
	Bullet* blt = new Bullet(position, radians);
	blt->setSpeed(m_bulletSpeed);
	blt->setRadius(m_bullet_radius);
	blt->setFillColor(sf::Color(
		m_bullet_color_r,
		m_bullet_color_g,
		m_bullet_color_b,
		m_bullet_color_a
	));
	m_bullets.push_back(blt);
}
void BulletManager::addBullet(Bullet* p_bullet)
{
	p_bullet->setRadius(m_bullet_radius);
	p_bullet->setFillColor(sf::Color(
		m_bullet_color_r,
		m_bullet_color_g,
		m_bullet_color_b,
		m_bullet_color_a
		));
	p_bullet->setOrigin(m_bullet_radius, m_bullet_radius);
	p_bullet->setSpeed(m_bulletSpeed);
	m_bullets.push_back(p_bullet);
}
void BulletManager::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	for (unsigned int i = 0; i < m_bullets.size(); i++)
	{
		rt.draw(*m_bullets[i], states);
	}
}