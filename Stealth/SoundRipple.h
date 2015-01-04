#pragma once

#include "SFML\System\Clock.hpp"
#include "SFML\Graphics\Drawable.hpp"
#include "SFML\Graphics\RenderTexture.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include "SFML\Graphics\CircleShape.hpp"
#include "Thor\Shapes\ConcaveShape.hpp"
#include "ClipperLib.h"

class SoundRipple: public sf::Drawable
{
public:
	SoundRipple();
	SoundRipple(sf::Vector2f position, float radius);
	~SoundRipple();

	void update(sf::Time deltatime);
	bool should_i_die();
	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
	void clipSound(const std::vector<ClipperLib::Path> &collision_bounds);
	bool canISeePoint(sf::Vector2f point);
	void setType(int type);
	sf::Vector2f getCenter();
	int getType();

private:
	sf::Clock m_lifeTime;
	bool m_can_i_die;
	sf::CircleShape shape;
	thor::ConcaveShape cshape;
	float m_radius;
	sf::Vector2f middle;
	std::vector<sf::Vertex> m_vertices;
	int type;
	ClipperLib::Path path;
	ClipperLib::Path current_path;
};

