#pragma once
#include "Math.h"

#include <SFML/System/Vector2.hpp>

namespace Vector
{
	template<typename T>
	T getLengthSquared(const sf::Vector2<T> &vector)
	{
		return (vector.x * vector.x + vector.y * vector.y);
	}
	template<typename T>
	float getLength(const sf::Vector2<T> &vector)
	{
		return Math::sqrt(getLengthSquared(vector));
	}

	template<typename T>
	void normalize(sf::Vector2<T> &vector)
	{
		float len = getLength(vector);
		if (len > 0.f)
		{
			float factor = 1.f / len;
			vector *= factor;
		}
	}
	template<typename T>
	sf::Vector2<T> getNormalized(sf::Vector2<T> vector)
	{
		normalize(vector);
		return vector;
	}

	template<typename T>
	inline sf::Vector2<T> interpolate(const sf::Vector2<T> &p, const sf::Vector2<T> &q, float f)
	{
		return sf::Vector2<T>(p.x*(1.f - f) + q.x*f, p.y*(1.f - f) + q.y*f);
	}
}

template<typename T>
sf::Vector2<T> &operator*=(sf::Vector2<T> &lhs, const sf::Vector2<T> &rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	return lhs;
}
template<typename T>
sf::Vector2<T> operator*(sf::Vector2<T> lhs, const sf::Vector2<T> &rhs)
{
	lhs *= rhs;
	return lhs;
}

template<typename T>
sf::Vector2<T> &operator/=(sf::Vector2<T> &lhs, const sf::Vector2<T> &rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	return lhs;
}
template<typename T>
sf::Vector2<T> operator/(sf::Vector2<T> lhs, const sf::Vector2<T> &rhs)
{
	lhs /= rhs;
	return lhs;
}
