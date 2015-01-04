#include "SoundRipple.h"
#include "Thor\Math\Triangulation.hpp"
#include <iostream>

SoundRipple::SoundRipple(sf::Vector2f position, float radius)
{
	this->m_radius = radius;
	this->shape = sf::CircleShape(this->m_radius);
	this->shape.setOrigin(this->m_radius, this->m_radius);
	this->shape.setPosition(position);
	this->cshape = thor::ConcaveShape();
	this->cshape.setFillColor(sf::Color(255, 255, 0, 90));
	this->m_can_i_die = false;
}

SoundRipple::SoundRipple()
{

}

SoundRipple::~SoundRipple()
{

}

void SoundRipple::update(sf::Time deltatime)
{
	if (m_lifeTime.getElapsedTime().asSeconds() > 0.5f)
	{
		m_can_i_die = true;
	}
}
bool SoundRipple::should_i_die()
{
	if (m_can_i_die)
		return true;
	return false;
}
void SoundRipple::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	if (cshape.getPointCount() > 2)
	{
		rt.draw(cshape);
	}
}
void SoundRipple::clipSound(const std::vector<ClipperLib::Path> &collision_bounds)
{	
	// Get the middle point
	middle.x = shape.getPosition().x - shape.getRadius();
	middle.y = shape.getPosition().y - shape.getRadius();

	// Create points from a sf::CircleShape
	ClipperLib::Path mask;
	for (unsigned int i = 0; i < shape.getPointCount(); i++)
	{
		sf::Vector2f pointPosition = shape.getPoint(i);
		mask.emplace_back(static_cast<ClipperLib::cInt>(middle.x + pointPosition.x), static_cast<ClipperLib::cInt>(middle.y + pointPosition.y));
	}

	middle.x += shape.getRadius();
	middle.y += shape.getRadius();

	// Setup clipper paths
	ClipperLib::Paths output;
	ClipperLib::Clipper clipper;
	clipper.Clear();
	if (collision_bounds.size() > 0)
	{
	for (unsigned int i = 0; i < collision_bounds.size(); i++)
	{
		clipper.AddPath(collision_bounds[i], ClipperLib::ptClip, true);
	}
	clipper.AddPath(mask, ClipperLib::ptSubject, true);
	clipper.Execute(ClipperLib::ctDifference, output, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	}
	path.clear();
	if (!output.empty())
	{

		// Choose which shape
		for (unsigned int k = 0; k < output.size(); k++)
		{
			std::vector<sf::Vector2f> m_vertices;
			for (unsigned int h = 0; h < output[k].size(); h++)
			{
				m_vertices.push_back(sf::Vector2f(static_cast<float>(output[k][h].X), static_cast<float>(output[k][h].Y)));
			}
			bool inside = false;
			for (unsigned int i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++)
			{
				const sf::Vector2f &vert1 = m_vertices[i];
				const sf::Vector2f &vert2 = m_vertices[j];

				if (
					((vert1.y > middle.y) != (vert2.y > middle.y)) &&
					(middle.x < ((vert2.x - vert1.x) * (middle.y - vert1.y) / (vert2.y - vert1.y) + vert1.x)))
				{
					inside = !inside;
				}
			}
			if (inside)
			{
				path = output[k];
				current_path = path;
				break;
			}
		}

		// Set point count in thor::ConcaveShape
		cshape.setPointCount(path.size());

		// Adds all points to the sf::ConvexShape
		for (unsigned int i = 0; i < path.size(); i++)
		{
			cshape.setPoint(i, sf::Vector2f(static_cast<float>(path[i].X), static_cast<float>(path[i].Y)));
		}

		/*
				std::cout << "Vertices for sound ripple shape:" << std::endl << std::endl;
				std::cout << "shape.setPointCount(" << cshape.getPointCount() << ");" << std::endl;
				for (int i = 0; i < cshape.getPointCount(); i++)
				{
				std::cout << "shape.setPoint("<<i<<", sf::Vector2f(" << cshape.getPoint(i).x << ", " << cshape.getPoint(i).y << "));" << std::endl;
				}*/
	}
}
bool SoundRipple::canISeePoint(sf::Vector2f point)
{
	bool inside = false;
	for (unsigned int i = 0, j = path.size() - 1; i < path.size(); j = i++)
	{
		const sf::Vector2f &vert1 = sf::Vector2f(path[i].X, path[i].Y);
		const sf::Vector2f &vert2 = sf::Vector2f(path[j].X, path[j].Y);

		if (
			((vert1.y > point.y) != (vert2.y > point.y)) &&
			(point.x < ((vert2.x - vert1.x) * (point.y - vert1.y) / (vert2.y - vert1.y) + vert1.x)))
		{
			inside = !inside;
		}
	}

	if (inside)
		return true;
	return false;
}

void SoundRipple::setType(int type)
{
	this->type = type;
}

int SoundRipple::getType()
{
	return type;
}

sf::Vector2f SoundRipple::getCenter()
{
	return middle;
}
