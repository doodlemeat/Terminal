#include "ObjectGroup.h"
#include <iostream>

ObjectGroup::ObjectGroup()
{
	objects.clear();
}

ObjectGroup::~ObjectGroup()
{
	auto it = objects.begin();
	while (it != objects.end())
	{
		delete *it;
		it = objects.erase(it);
	}
}


std::vector<MapObject*> ObjectGroup::getObjectsByType(std::string identifier)
{
	std::vector<MapObject*> o;
	o.clear();
	auto it = objects.begin();
	while (it != objects.end())
	{
		if ((*it)->type == identifier)
		{
			o.push_back(*it);
		}
		++it;
	}
	if (o.size() == 0)
		std::cout << "No objects was found with type " << identifier << std::endl;
	return o;
}

std::vector<MapObject*> ObjectGroup::getObjectsByName(std::string identifier)
{
	std::vector<MapObject*> o;
	o.clear();
	auto it = objects.begin();
	while (it != objects.end())
	{
		if ((*it)->name == identifier)
		{
			o.push_back(*it);
		}
		++it;
	}
	if (o.size() == 0)
		std::cout << "No objects was found with name " << identifier << std::endl;
	return o;
}

MapObject::MapObject()
{

}
MapObject::~MapObject()
{

}
sf::Vector2f MapObject::getPosition()
{
	return sf::Vector2f(x, y);
}
float MapObject::propertyFloat(std::string name)
{
	if (m_properties.find(name) != m_properties.end())
		return std::atof(m_properties[name].c_str());
	else
		return 0.f;
}
int MapObject::propertyInt(std::string name)
{
	if (m_properties.find(name) != m_properties.end())
		return std::atoi(m_properties[name].c_str());
	else
		return 0;
}
bool MapObject::propertyBool(std::string name)
{
	if (m_properties.find(name) != m_properties.end())
	{
		if (m_properties[name] == "true")
			return true;
	}
	return false;
}

bool MapObject::hasProperty(std::string p_key)
{
	auto it = m_properties.find(p_key);
	if (it != m_properties.end())
	{
		return true;
	}
	return false;
}

std::string MapObject::getProperty(std::string p_key)
{
	auto it = m_properties.find(p_key);
	if (it != m_properties.end())
	{
		return it->second;
	}
	return "";
}

MapObjectRectangle::MapObjectRectangle()
{

}
MapObjectRectangle::~MapObjectRectangle()
{

}
MapObjectPolyline::MapObjectPolyline()
{

}
MapObjectPolyline::~MapObjectPolyline()
{

}