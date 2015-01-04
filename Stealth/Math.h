#pragma once

#include <cmath>
#include "SFML\System\Vector2.hpp"

namespace Math
{
	static const float PI = 3.14159265358979323846f;
	static const float PI_2 = 6.28318530717958647693f;
	static const float PI_HALF = 1.57079632679489661923f;

	// All trig functions expect values in radians
	inline float sin(float value)        { return std::sin(value); }
	inline float cos(float value)        { return std::cos(value); }
	inline float tan(float value)        { return std::tan(value); }
	inline float asin(float value)       { return std::asin(value); }
	inline float acos(float value)       { return std::acos(value); }
	inline float atan(float value)       { return std::atan(value); }
	inline float atan2(float y, float x) { return std::atan2(y, x); }

	inline float sqrt(float value)  { return std::sqrt(value); }
	inline float pow(float base, float exponent) { return std::pow(base, exponent); }
	inline float floor(float value) { return std::floor(value); }
	inline int floorToInt(float value) { return static_cast<int>(floor(value)); }
	inline float ceil(float value)  { return std::ceil(value); }
	inline int ceilToInt(float value) { return static_cast<int>(ceil(value)); }
	inline float round(float value) { return floor(value + 0.5f); }
	inline int roundToInt(float value) { return static_cast<int>(round(value)); }
	inline float abs(float value)   { return std::abs(value); }
	inline int abs(int value)       { return std::abs(value); }
	inline float log(float value)   { return std::log(value); }
	inline int log(int value)       { return std::log(value); }
	inline float dot(sf::Vector2f v1, sf::Vector2f v2) { return (v1.x * v2.x) + (v1.y * v2.y); }
	
	template<typename T>
	T clamp(T value, T min, T max)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}
	template<typename T>
	T wrap(T value, T min, T max)
	{
		T range = max - min + 1;
		while (value < min)
			value += range;
		while (value > max)
			value -= range;
		return value;
	}

	template<typename T>
	inline T min(T a, T b)
	{
		return (a < b ? a : b);
	}
	template<typename T>
	inline T max(T a, T b)
	{
		return (a < b ? b : a);
	}

	// returns a difference between to angle in range [0, 180]
	inline float angle_difference(float a, float b)
	{
		float dist = fmodf(a - b + 360.f, 360.f);
		if (dist > 180.f)
			dist = 360.f - dist;
		return dist;
	}

	// returns a boolen representing which direction to rotate between 2 vectors
	// true - rotate positive
	inline bool directionOfRotation(float a, float b)
	{
		return (fmodf(a - b + 360.f, 360.f) > 180.f);
	}

	// get the angle to object b from object a
	inline float getAngleFrom(sf::Vector2f a, sf::Vector2f b)
	{
		float dy = a.y - b.y;
		float dx = a.x - b.x;
		return atan2f(-dy, -dx) * 180.f / 3.141592f + 180.f;
	}
}