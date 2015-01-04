#pragma once

#include <vector>

#include "SFML\System\Vector2.hpp"
#include "SFML\Graphics\Sprite.hpp"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include "ClipperLib.h"
#include "SFML\Graphics\Vertex.hpp"
#include "SFML\Graphics\VertexArray.hpp"

// Forward declare segment
struct Segment;

struct EndPoint
{
	float x;
	float y;
	bool begin;
	Segment *segment;
	float angle;
	bool visualize;
};

struct Segment
{
	EndPoint* a;
	EndPoint* b;
	float d;
};

struct Triangle
{
	Triangle(sf::Vector2f v1, sf::Vector2f v2, sf::Vector2f v3) : p1(v1), p2(v2), p3(v3) { }
	sf::Vector2f p1;
	sf::Vector2f p2;
	sf::Vector2f p3;
};

// A structure that defines a future segment.
// A list of PrepareSegment is hold by the lightengine
// That list will be used to create segments when a new light is created
struct PrepareSegment
{
	sf::Vector2f a;
	sf::Vector2f b;
};

class Light: public sf::Drawable
{
public:
	Light(sf::Vector2f position, std::vector<sf::Vector2f> fov_points, float angle);
	void setLocation(); // this is called from the Light::update()
	void changePosition(sf::Vector2f p_position);
	void setColor(sf::Color color);
	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
	void update(sf::Vector2f position, float angle); // this method updates the angle and position
	void addSegment(sf::Vector2f p1, sf::Vector2f p2); // adds 1 segment and 2 endpoints to the lists
	bool detectPlayer(sf::Vector2f point);
	bool isDead()
	{
		return m_dead;
	}
	void setDead(bool p_value)
	{
		m_dead = p_value;
	}

	std::vector<EndPoint*> m_endPoints; // list of endpoints
	std::vector<Segment*> m_segments; // list of segments

	ClipperLib::Path m_lightPoly;
	ClipperLib::Path maskPoly;
	std::vector<sf::Vertex> m_vertices;

	std::vector<sf::Vector2f> m_fov_points; // This is not used atm. This will be used to define the vision field instead of hardcoded values
	sf::Color m_current_color; // Current color of the light
	sf::Color m_color; // Color of the light
	sf::Color m_color_detected; // Color of light when player detected
	float m_angle; // The angle the light points at
	sf::Vector2f m_position; // The global position of the light
	bool m_dead; // Draw light or not
};

class LightEngine: public sf::Drawable
{
public:
	LightEngine();
	~LightEngine();


	// Helper functions
	void prepareSegment(sf::Vector2f p1, sf::Vector2f p2);
	bool segment_in_front_of(Segment *a, Segment *b, sf::Vector2f relativeTo);
	bool leftOf(Segment *a, sf::Vector2f point);
	sf::Vector2f interpolate(sf::Vector2f p, sf::Vector2f q, float f);
	sf::Vector2f linesIntersection(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4);
	sf::Vector2f LightEngine::RotatePoint(sf::Vector2f pointToRotate, sf::Vector2f centerPoint, double angleInDegrees);

	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
	void update(std::vector<Light*> lights);
	Light* addLight(sf::Vector2f position, std::vector<sf::Vector2f> fov_points, float angle);
	void addTriangle(Light* light, float angle1, float angle2, Segment *segment);

	std::vector<PrepareSegment*> m_prep_segments;
	std::vector<Light*> m_lights;
};

