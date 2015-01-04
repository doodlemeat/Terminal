#pragma once

#include "SFML\Graphics\Drawable.hpp"
#include "SFML\System\Time.hpp"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include "ClipperLib.h"
#include <vector>

enum SoundRippleType
{
	RIPPLE_UNKNOWN,
	RIPPLE_PLAYER
};
class SoundRipple;
class SoundrippleManager: public sf::Drawable
{
public:
	SoundrippleManager();
	~SoundrippleManager();

	void update(sf::Time deltatime);
	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
	void createRipple(sf::Vector2f position, float radius, SoundRippleType type);
	void setCollisionBounds(std::vector<sf::FloatRect> collisionBounds);
	bool canPointHearMe(sf::Vector2f point, SoundRippleType type);
	bool canPointHearMe(sf::Vector2f point, SoundRippleType type, SoundRipple& refRipple);

private:
	std::vector<ClipperLib::Path> m_collisionPolygon;
	std::vector<SoundRipple*> m_soundRipples;
};

