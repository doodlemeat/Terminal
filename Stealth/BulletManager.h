#pragma once

#include <vector>
#include "SFML\System\Time.hpp"
#include "SFML\System\Vector2.hpp"
#include "SFML\Graphics\Drawable.hpp"
#include "SFML\Graphics\CircleShape.hpp"

class Guard;
class Player;

class Bullet: public sf::CircleShape
{
public:
	Bullet(sf::Vector2f position, float radians);
	~Bullet();

	void update(sf::Time deltatime);
	void setSpeed(float p_speed)
	{
		this->speed = p_speed;
	}

	void setDie(bool p_value)
	{
		m_die = p_value;
	}

	void setDamage(int p_value)
	{
		damage = p_value;
	}

	int getDamage()
	{
		return damage;
	}

	bool getDie()
	{
		return m_die;
	}
	sf::Transform m_transform;
private:
	sf::Vector2f direction;
	float speed;
	bool m_die;
	int damage;
};

class BulletManager: public sf::Drawable
{
public:
	BulletManager();
	~BulletManager();

	void update(Player* player, std::vector<Guard*> &guards, std::vector<sf::FloatRect> collision_bounds, sf::Time deltatime);
	void spawnBullet(sf::Vector2f position, float radians);
	void addBullet(Bullet* p_bullet);
	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;

private:
	float m_bullet_radius;
	float m_bulletSpeed;
	int m_bullet_color_r;
	int m_bullet_color_g;
	int m_bullet_color_b;
	int m_bullet_color_a;
	std::vector<Bullet*> m_bullets;
};

