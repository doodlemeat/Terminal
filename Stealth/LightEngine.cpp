#include "LightEngine.h"
#include "Math.h"
#include "ClipperLib.h"
#include "Angle.h"
#include "SFML\Graphics\CircleShape.hpp"
#include "SFML\Graphics\ConvexShape.hpp"
#include "boost\assert.hpp"
#include "Config.h"

#include <iostream>

// This function sorts endpoints relative to the lightsource centrum
bool sortEndPoints(EndPoint* a, EndPoint* b)
{
	if (a->angle > b->angle)
		return false;
	if (a->angle < b->angle)
		return true;

	if (!a->begin && b->begin)
		return false;
	if (a->begin && !b->begin)
		return true;
	return false;
}


Light::Light(sf::Vector2f position, std::vector<sf::Vector2f> fov_points, float angle)
{
	m_position = position;
	m_fov_points = fov_points;
	m_angle = (angle * Angle::DEG2RAD);
	if (!m_dead && !m_vertices.empty())
	m_dead = false;
}

void Light::setLocation()
{
	for (auto &segment : m_segments)
	{
		float dx = 0.5f * (segment->a->x + segment->b->x) - m_position.x;
		float dy = 0.5f * (segment->a->y + segment->b->y) - m_position.x;

		segment->d = dx*dx + dy*dy;

		//float angle = acosf(x / segment->d);
		segment->a->angle = Math::atan2(segment->a->y - m_position.y, segment->a->x - m_position.x);
		segment->b->angle = Math::atan2(segment->b->y - m_position.y, segment->b->x - m_position.x);

		float dAngle = segment->b->angle - segment->a->angle;
		if (dAngle <= -Math::PI) { dAngle += 2.f * Math::PI; }
		if (dAngle > Math::PI) { dAngle -= 2.f * Math::PI; }
		segment->a->begin = (dAngle > 0.0f);
		segment->b->begin = !segment->a->begin;
	}
}
void Light::changePosition(sf::Vector2f p_position)
{
	m_position = p_position;
}
void Light::setColor(sf::Color color)
{
	m_current_color = color;
}
void Light::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	if (!m_dead && !m_vertices.empty())
	{
		rt.draw(&m_vertices[0], static_cast<unsigned int>(m_vertices.size()), sf::PrimitiveType::TrianglesFan, states);	
	}
}
void Light::update(sf::Vector2f position, float angle)
{
	m_position.x = position.x;
	m_position.y = position.y;
	m_angle = angle * Angle::DEG2RAD;
}
bool Light::detectPlayer(sf::Vector2f point)
{
	bool inside = false;
	for (unsigned int i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++)
	{
		const sf::Vector2f &vert1 = m_vertices[i].position;
		const sf::Vector2f &vert2 = m_vertices[j].position;

		if (
			((vert1.y > point.y) != (vert2.y > point.y)) &&
			(point.x < ((vert2.x - vert1.x) * (point.y - vert1.y) / (vert2.y - vert1.y) + vert1.x)))
		{
			inside = !inside;
		}
	}
	if (inside)
	{
		setColor(m_color_detected);
		return true;
	}
	else
	{
		setColor(m_color);
	}
	return false;
}

LightEngine::LightEngine()
{
	m_prep_segments.clear();
	m_lights.clear();
}
LightEngine::~LightEngine()
{
}
void LightEngine::update(std::vector<Light*> lights)
{
	for (auto &light : lights)
	{
		light->setLocation();
		std::sort(light->m_endPoints.begin(), light->m_endPoints.end(), sortEndPoints);
		
		std::vector<Segment*> open;
		float startingAngle = 0.f;

		light->m_lightPoly.clear();
		for (unsigned int i = 0; i <= 1; i++)
		{
			for (auto &p : light->m_endPoints)
			{
				Segment* current_old = open.empty() ? nullptr : open[0];

				
				if (p->begin)
				{
					auto it = open.begin();
					while (it != open.end() && segment_in_front_of(p->segment, (*it), light->m_position))
					{
						++it;
					}
					if (it == open.end())
					{
						open.push_back(p->segment);
					}
					else
					{
						open.insert(it, p->segment);
					}
				}
				else
				{
					auto it = std::find(open.begin(), open.end(), p->segment);
					if (it != open.end())
					{
						open.erase(it);
					}
				}

				Segment* current_new = open.empty() ? nullptr : open.front();
				if (current_old != current_new)
				{
					if (i == 1)
					{
						addTriangle(light, startingAngle, p->angle, current_old);
					}
					startingAngle = p->angle;
				}
			}
		}
		
		// Create the mask
		sf::Vector2f p1(light->m_position.x, light->m_position.y);
		sf::Vector2f p2((400.f + light->m_position.x), (-200.f + light->m_position.y));
		sf::Vector2f p4((400.f + light->m_position.x), (200.f + light->m_position.y));

		// Rotate the outer points around the center point
		p2 = RotatePoint(p2, p1, light->m_angle);
		p4 = RotatePoint(p4, p1, light->m_angle);

		// Create and fill the mask with updated and rotated points
		light->maskPoly.clear();
		light->maskPoly.emplace_back(static_cast<ClipperLib::cInt>(p1.x * 1000.f), static_cast<ClipperLib::cInt>(p1.y * 1000.f));
		light->maskPoly.emplace_back(static_cast<ClipperLib::cInt>(p2.x * 1000.f), static_cast<ClipperLib::cInt>(p2.y * 1000.f));
		light->maskPoly.emplace_back(static_cast<ClipperLib::cInt>(p4.x * 1000.f), static_cast<ClipperLib::cInt>(p4.y * 1000.f));

		// Intersect with mask
		ClipperLib::Paths output;
		if (light->maskPoly.empty())
		{
			output.push_back(light->m_lightPoly);
		}
		else
		{
			ClipperLib::Clipper clipper;
			clipper.Clear();
			clipper.AddPath(light->m_lightPoly, ClipperLib::ptSubject, true);
			clipper.AddPath(light->maskPoly, ClipperLib::ptClip, true);
			clipper.Execute(ClipperLib::ctIntersection, output, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
		}

		// Fill the vertex array
		light->m_vertices.clear();
		if (!output.empty())
		{
			light->m_vertices.emplace_back(sf::Vector2f(light->m_position.x, light->m_position.y), light->m_current_color);
			const ClipperLib::Path &path = output[0]; // We only care about the first shape
			
			for (const ClipperLib::IntPoint &point : path)
			{
				sf::Vector2f tp(static_cast<float>(point.X) / 1000.f, static_cast<float>(point.Y) / 1000.f);
				light->m_vertices.emplace_back(tp, light->m_current_color);
			}
			// Add first point again to close the shape
			{
				const ClipperLib::IntPoint &point = path[0];
				sf::Vector2f tp(static_cast<float>(point.X) / 1000.f, static_cast<float>(point.Y) / 1000.f);
				light->m_vertices.emplace_back(tp, light->m_current_color);
			}
		}
	}
}

void LightEngine::prepareSegment(sf::Vector2f p1, sf::Vector2f p2)
{
	PrepareSegment *prep = new PrepareSegment();
	prep->a = p1;
	prep->b = p2;
	m_prep_segments.push_back(prep);
}

void Light::addSegment(sf::Vector2f p1, sf::Vector2f p2)
{
	Segment* segment = new Segment();
	EndPoint* ep1 = new EndPoint();
	EndPoint* ep2 = new EndPoint();

	ep1->begin = false;
	ep1->x = p1.x;
	ep1->y = p1.y;
	ep1->segment = segment;
	ep1->angle = 0.0f;
	ep1->visualize = true;

	ep2->begin = false;
	ep2->x = p2.x;
	ep2->y = p2.y;
	ep2->segment = segment;
	ep2->angle = 0.0f;
	ep2->visualize = false;

	segment->a = ep1;
	segment->b = ep2;
	segment->d = 0.0f;
	m_segments.push_back(segment);
	m_endPoints.push_back(ep1);
	m_endPoints.push_back(ep2);
}

Light* LightEngine::addLight(sf::Vector2f position, std::vector<sf::Vector2f> fov_points, float angle)
{
	Light *light = new Light(position, fov_points, angle);
	for (auto &psegment: m_prep_segments)
	{
		light->addSegment(psegment->a, psegment->b);
	}
	light->addSegment(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(9000.0f, 0.0f));
	light->addSegment(sf::Vector2f(9000.0f, 0.0f), sf::Vector2f(9000.0f, 9000.0f));
	light->addSegment(sf::Vector2f(9000.0f, 9000.0f), sf::Vector2f(0.0f, 9000.0f));
	light->addSegment(sf::Vector2f(0.0f, 9000.0f), sf::Vector2f(0.0f, 0.0f));
	m_lights.push_back(light);
	return light;
}

bool LightEngine::segment_in_front_of(Segment *a, Segment *b, sf::Vector2f relativeTo)
{
	bool A1 = leftOf(a, interpolate(sf::Vector2f(b->a->x, b->a->y), sf::Vector2f(b->b->x, b->b->y), 0.01f));
	bool A2 = leftOf(a, interpolate(sf::Vector2f(b->b->x, b->b->y), sf::Vector2f(b->a->x, b->a->y), 0.01f));
	bool A3 = leftOf(a, relativeTo);

	bool B1 = leftOf(b, interpolate(sf::Vector2f(a->a->x, a->a->y), sf::Vector2f(a->b->x, a->b->y), 0.01f));
	bool B2 = leftOf(b, interpolate(sf::Vector2f(a->b->x, a->b->y), sf::Vector2f(a->a->x, a->a->y), 0.01f));
	bool B3 = leftOf(b, relativeTo);

	if (B1 == B2 && B2 != B3) return true;
	if (A1 == A2 && A2 == A3) return true;
	if (A1 == A2 && A2 != A3) return false;
	if (B1 == B2 && B2 == B3) return false;
	return false;
}

bool LightEngine::leftOf(Segment *s, sf::Vector2f point)
{
	float cross = (s->b->x - s->a->x) * (point.y - s->a->y) 
				- (s->b->y - s->a->y) * (point.x - s->a->x);
	return cross < 0;
}

sf::Vector2f LightEngine::interpolate(sf::Vector2f p, sf::Vector2f q, float f) {
	return sf::Vector2f(p.x * (1.f - f) + q.x * f, p.y * (1.f - f) + q.y * f);
}

sf::Vector2f LightEngine::linesIntersection(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4)
{
	float s = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x))
		/ ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));
	return sf::Vector2f(p1.x + s * (p2.x - p1.x), p1.y + s * (p2.y - p1.y));
}

void LightEngine::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	for (auto &light : m_lights)
	{
		rt.draw(*light);

		if (Config::debug)
		{
			// Draw vertices points
			/*for (const sf::Vertex &vertex : light->m_vertices)
			{
				sf::CircleShape circle(5.f);
				circle.setOrigin(5.f, 5.f);
				circle.setPosition(vertex.position);
				rt.draw(circle, states);

				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(light->m_position.x, light->m_position.y)),
					sf::Vertex(sf::Vector2f(vertex.position.x, vertex.position.y))
				};
				rt.draw(line, 2, sf::PrimitiveType::Lines);
			}*/
			
			//// Draw all the segments
			//for (auto &segment : light->m_segments)
			//{
			//	sf::Vertex line[] =
			//	{
			//		sf::Vertex(sf::Vector2f(segment->a->x, segment->a->y)),
			//		sf::Vertex(sf::Vector2f(segment->b->x, segment->b->y))
			//	};
			//	rt.draw(line, 2, sf::PrimitiveType::Lines);
			//}
		}
	}
}


void LightEngine::addTriangle(Light* light, float angle1, float angle2, Segment *segment)
{
	sf::Vector2f p1 = light->m_position;
	sf::Vector2f p2(p1.x + Math::cos(angle1), p1.y + Math::sin(angle1));
	sf::Vector2f p3(0.f, 0.f);
	sf::Vector2f p4(0.f, 0.f);

	if (segment != nullptr)
	{
		p3.x = segment->a->x;
		p3.y = segment->a->y;
		p4.x = segment->b->x;
		p4.y = segment->b->y;
	}
	else {
		p3.x = p1.x + Math::cos(angle1) * 500;
		p3.y = p1.y + Math::sin(angle1) * 500;
		p4.x = p1.x + Math::cos(angle2) * 500;
		p4.y = p1.y + Math::sin(angle2) * 500;
	}

	sf::Vector2f pBegin = linesIntersection(p3, p4, p1, p2);
	p2.x = p1.x + Math::cos(angle2);
	p2.y = p1.y + Math::sin(angle2);
	sf::Vector2f pEnd = linesIntersection(p3, p4, p1, p2);

	sf::Transform inverse;
	sf::Vector2f tp1(inverse * pBegin);
	sf::Vector2f tp2(inverse * pEnd);

	light->m_lightPoly.emplace_back(static_cast<ClipperLib::cInt>(tp1.x * 1000.f + p1.x), static_cast<ClipperLib::cInt>(tp1.y * 1000.f + p1.y));
	light->m_lightPoly.emplace_back(static_cast<ClipperLib::cInt>(tp2.x * 1000.f + p1.x), static_cast<ClipperLib::cInt>(tp2.y * 1000.f + p1.y));
}

// Simple method to rotate a point around another point
sf::Vector2f LightEngine::RotatePoint(sf::Vector2f pointToRotate, sf::Vector2f centerPoint, double angleInRadians)
{
	double cosTheta = cos(angleInRadians);
	double sinTheta = sin(angleInRadians);
	return sf::Vector2f((static_cast<float>(cosTheta) * (pointToRotate.x - centerPoint.x) -
		static_cast<float>(sinTheta) * (pointToRotate.y - centerPoint.y) + centerPoint.x),
		(static_cast<float>(sinTheta) * (pointToRotate.x - centerPoint.x) +
		static_cast<float>(cosTheta) * (pointToRotate.y - centerPoint.y) + centerPoint.y)
		);
}