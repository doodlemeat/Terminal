#pragma once

#include <string>
#include <vector>

#include "SFML\Graphics.hpp"

enum MapObjectTypes
{
	MAPOBJECT_RECTANGLE,
	MAPOBJECT_POLYLINE
};

class MapObject
{
public:
	MapObject();
	virtual ~MapObject();

	std::string name;
	std::string type;
	float x;
	float y;
	float width;
	float height;

	sf::Vector2f getPosition();
	std::map<std::string, std::string> m_properties;

	float propertyFloat(std::string name);
	int propertyInt(std::string name);
	bool propertyBool(std::string name);

	bool hasProperty(std::string p_key);
	std::string MapObject::getProperty(std::string p_key);

	MapObjectTypes mapObjectType;
};

////////////////////////////////

class MapObjectRectangle: public MapObject
{
public:
	MapObjectRectangle();
	~MapObjectRectangle();
};

/////////////////////////////////

class MapObjectPolyline: public MapObject
{
public:
	MapObjectPolyline();
	~MapObjectPolyline();
	std::vector<sf::Vector2f> points;
};

/////////////////////////////////

class ObjectGroup
{
public:
	ObjectGroup();
	~ObjectGroup();

	std::vector<MapObject*> getObjectsByName(std::string identifier);
	std::vector<MapObject*> getObjectsByType(std::string identifier);
	
	template<typename T>
	T getObjectByType(const std::string identifier)
	{
		auto it = objects.begin();
		while (it != objects.end())
		{
			if ((*it)->type == identifier)
			{
				return dynamic_cast<T>(*it);
			}
			++it;
		}
		std::cout << "Failed to find object with type " << identifier << std::endl;
		return nullptr;
	}
	template<typename T>
	T getObjectByName(const std::string identifier)
	{
		auto it = objects.begin();
		while (it != objects.end())
		{
			if ((*it)->name == identifier)
			{
				return dynamic_cast<T>(*it);
			}
			++it;
		}
		std::cout << "Failed to find object with name " << identifier << std::endl;
		return nullptr;
	}

	std::string name;
	int width;
	int height;
	std::vector<MapObject*> objects;
};