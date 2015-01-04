#include "SoundrippleManager.h"
#include "SoundRipple.h"
#include <iostream>

SoundrippleManager::SoundrippleManager()
{

}
SoundrippleManager::~SoundrippleManager()
{
	for (unsigned int i = 0; i < m_soundRipples.size(); i++)
	{
		delete m_soundRipples[i];
	}
	m_soundRipples.clear();
}

void SoundrippleManager::update(sf::Time deltatime)
{
	for (unsigned int i = 0; i < m_soundRipples.size(); i++)
	{
		if (m_soundRipples[i]->should_i_die())
		{
			delete m_soundRipples[i];
			m_soundRipples.erase(m_soundRipples.begin() + i);
			continue;
		}
		m_soundRipples[i]->update(deltatime);
	}
}
void SoundrippleManager::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	for (auto &ripple: m_soundRipples)
	{
		rt.draw(*ripple);
	}
}
void SoundrippleManager::createRipple(sf::Vector2f position, float radius, SoundRippleType type)
{
	SoundRipple* soundRipple = new SoundRipple(position, radius);
	soundRipple->clipSound(m_collisionPolygon);
	soundRipple->setType(type);
	m_soundRipples.push_back(soundRipple);
}
void SoundrippleManager::setCollisionBounds(std::vector<sf::FloatRect> collisionsBounds)
{
	std::vector<ClipperLib::Path> m_collisionBounds;
	for (unsigned int i = 0; i < collisionsBounds.size(); i++)
	{
		ClipperLib::Path path;
		path.emplace_back(static_cast<ClipperLib::cInt>(collisionsBounds[i].left), static_cast<ClipperLib::cInt>(collisionsBounds[i].top));
		path.emplace_back(static_cast<ClipperLib::cInt>(collisionsBounds[i].left + collisionsBounds[i].width), static_cast<ClipperLib::cInt>(collisionsBounds[i].top));
		path.emplace_back(static_cast<ClipperLib::cInt>(collisionsBounds[i].left + collisionsBounds[i].width), static_cast<ClipperLib::cInt>(collisionsBounds[i].top + collisionsBounds[i].height));
		path.emplace_back(static_cast<ClipperLib::cInt>(collisionsBounds[i].left), static_cast<ClipperLib::cInt>(collisionsBounds[i].top + collisionsBounds[i].height));
		m_collisionBounds.push_back(path);
	}
	ClipperLib::Paths mergedWalls;
	ClipperLib::Clipper mergeClipper;
	mergeClipper.Clear();
	for (unsigned int i = 0; i < m_collisionBounds.size(); i++)
	{
		mergeClipper.AddPath(m_collisionBounds[i], ClipperLib::ptSubject, true);
	}
	mergeClipper.Execute(ClipperLib::ctUnion, mergedWalls, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	for (unsigned int i = 0; i < mergedWalls.size(); i++)
	{
		m_collisionPolygon.push_back(mergedWalls[i]);
	}
}

bool SoundrippleManager::canPointHearMe(sf::Vector2f point, SoundRippleType type)
{
	for (unsigned int i = 0; i < m_soundRipples.size(); i++)
	{
		if (m_soundRipples[i]->canISeePoint(point) && m_soundRipples[i]->getType() == type)
		{
			return true;
		}
	}
	return false;
}

bool SoundrippleManager::canPointHearMe(sf::Vector2f point, SoundRippleType type, SoundRipple& refRipple)
{
	for (unsigned int i = 0; i < m_soundRipples.size(); i++)
	{
		if (m_soundRipples[i]->canISeePoint(point) && m_soundRipples[i]->getType() == type)
		{
			refRipple = *m_soundRipples[i];
			return true;
		}
	}
	return false;
}