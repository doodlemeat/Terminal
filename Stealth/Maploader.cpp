#include "Maploader.h"
#include "SpriteManager.h"
#include <iostream>
#include "String.h"
#include "Pathfinder.h"
#include "LightEngine.h"
#include "ObjectGroup.h"
#include "boost\assert.hpp"

Maploader::Maploader(std::string path)
{
	m_mapPath = path;
	m_tilesetTextures.clear();
	m_objectgroups.clear();
	m_layers.clear();
	m_mapLoaded = false;
}

Maploader::~Maploader()
{
	auto it = m_objectgroups.begin();
	while (it != m_objectgroups.end())
	{
		delete *it;
		it = m_objectgroups.erase(it);
	}
}

bool Maploader::Load(Pathfinder* pathfinder)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError fileLoaded = doc.LoadFile(m_mapPath.c_str());
	if (fileLoaded != 0)
	{
		std::cout << "Failed to open " << m_mapPath << std::endl;
		return m_mapLoaded = false;
	}

	mapNode = doc.FirstChildElement("map");
	if (mapNode == nullptr)
	{
		std::cout << "Map node not found. Map " << m_mapPath << " not loaded." << std::endl;
		return m_mapLoaded = false;
	}

	if (!(m_mapLoaded = ParseMap())) return false;
	if (!(m_mapLoaded = ParseTilesets())) return false;

	tinyxml2::XMLNode* currentNode = mapNode->FirstChild();
	std::vector<Node> nodes;
	int col = 0;
	int row = 0;
	for (int i = 0; i < (m_width * m_height); i++)
	{
		Node n;
		n.x = col;
		n.y = row;
		n.walkable = true;
		n.parent = nullptr;
		n.f = 0;
		n.g = 0;
		n.closed = false;
		nodes.push_back(n);
		
		col++;
		if (col == m_width)
		{
			col = 0;
			row++;
		}
	}
	while (currentNode != nullptr)
	{
		tinyxml2::XMLElement* cn = currentNode->ToElement();
		std::string name = cn->Name();
		if (name == "layer")
		{
			if (!(m_mapLoaded = ParseLayer(cn, nodes)))
			{
				Unload();
				return false;
			}
		}
		else if (name == "objectgroup")
		{
			if (!(m_mapLoaded = ParseObjectGroup(cn)))
			{
				Unload();
				return false;
			}
		}
		currentNode = currentNode->NextSibling();
		if (currentNode == nullptr)
		{
			printf("NO MORE LAYERS\n");
		}
	}
	pathfinder->retrieveNodes(nodes, m_width, m_height, m_tileWidth, m_tileHeight);
	std::cout << "Parsed " << m_layers.size() << " tile layer(s)." << std::endl;
	std::cout << "Parsed " << m_objectgroups.size() << " object group(s)" << std::endl;
	std::cout << "Loaded " << m_mapPath << " successfully." << std::endl;

	return m_mapLoaded = true;
}
bool Maploader::ParseMap()
{
	m_width = std::atoi(mapNode->Attribute("width"));
	m_height = std::atoi(mapNode->Attribute("height"));
	m_tileWidth = std::atoi(mapNode->Attribute("tilewidth"));
	m_tileHeight = std::atoi(mapNode->Attribute("tileheight"));

	tinyxml2::XMLElement* map_property = mapNode->FirstChildElement("properties");
	if (map_property != nullptr)
	{
		map_property = map_property->FirstChildElement("property");

		while (map_property != nullptr)
		{
			std::string name = map_property->Attribute("name");
			std::string value = map_property->Attribute("value");
			m_properties[name] = value;
			map_property = map_property->NextSiblingElement("property");
			std::cout << "Added map property " << name << " with value " << value << std::endl;
		}
	}
	return true;
}
bool Maploader::ParseTilesets()
{
	tinyxml2::XMLElement* tileset = mapNode->FirstChildElement("tileset");
	if (tileset == nullptr)
	{
		std::cout << "No tilesets found." << std::endl;
		return false;
	}
	std::cout << "Caching image files, please wait..." << std::endl;

	m_tileInfo.push_back(TileInfo());
	while (tileset != nullptr)
	{
		if (!ProcessTiles(tileset)) return false;
		tileset = tileset->NextSiblingElement("tileset");
	}
	return true;
}
bool Maploader::ProcessTiles(tinyxml2::XMLElement* tilesetNode)
{
	int tileWidth;
	int tileHeight;
	int spacing;
	int margin;

	// tr and parse tile sizes
	if (!(tileWidth = tilesetNode->IntAttribute("tilewidth")) || !(tileHeight = tilesetNode->IntAttribute("tileheight")))
	{
		std::cout << "Invalid tileset data found. Map not loaded." << std::endl;
		Unload();
		return false;
	}
	spacing = (tilesetNode->Attribute("spacing")) ? tilesetNode->IntAttribute("spacing") : 0;
	margin = (tilesetNode->Attribute("spacing")) ? tilesetNode->IntAttribute("margin") : 0;

	// try parsing image node
	tinyxml2::XMLElement* imageNode;
	if (!(imageNode = tilesetNode->FirstChildElement("image")) || !(imageNode->Attribute("source")))
	{
		std::cout << "Missing image data in tmx file. Map not loaded." << std::endl;
		Unload();
		return false;
	}

	// process image from disk
	sf::Texture* sourceTexture = nullptr;
	std::string imagePath = imageNode->Attribute("source");
	sourceTexture = SpriteManager::loadImage(imagePath);
	if (sourceTexture == nullptr)
	{
		std::cout << "Failed to load image " << imagePath << std::endl;
		return false;
	}
	m_tilesetTextures.push_back(sourceTexture);

	// slice into tiles
	int columns = (sourceTexture->getSize().x - margin) / (tileWidth + spacing);
	int rows = (sourceTexture->getSize().y - margin) / (tileHeight + spacing);

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			sf::IntRect rect;
			rect.top = y * (tileHeight + spacing);
			rect.top += margin;
			rect.height = tileHeight;
			rect.left = x * (tileWidth + spacing);
			rect.left += margin;
			rect.width = tileWidth;

			m_tileInfo.push_back(TileInfo(rect, 
				sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height)),
				m_tilesetTextures.size() - 1u));
		}
	}

	std::cout << "Processed " << imagePath << std::endl;
	return true;
}
bool Maploader::ParseLayer(tinyxml2::XMLElement* layerNode, std::vector<Node> &nodes)
{
	std::cout << "Found standard map layer " << layerNode->Attribute("name") << std::endl;
	
	MapLayer layer(Layer);
	if (layerNode->Attribute("name")) layer.name = layerNode->Attribute("name");
	layer.visible = true;
	if (layerNode->Attribute("opacity")) layer.opacity = layerNode->FloatAttribute("opacity"); if (!layerNode->Attribute("opacity")) layer.opacity = 1.f;
	std::cout << layer.visible << std::endl;
	tinyxml2::XMLElement* dataNode = layerNode->FirstChildElement("data");
	if (dataNode == nullptr)
	{
		std::cout << "Layer data missing or corrupt. Map not loaded." << std::endl;
		return false;
	}

	std::cout << "Found unencoded XML data." << std::endl;

	tinyxml2::XMLElement* tileElement = dataNode->FirstChildElement("tile");
	if (tileElement == nullptr)
	{
		std::cout << "No tile data found. Map not loaded." << std::endl;
		return false;
	}

	if (tinyxml2::XMLElement* propertiesNode = layerNode->FirstChildElement("properties"))
		ParseLayerProperties(propertiesNode, layer);

	int x = 0;
	int y = 0;
	while (tileElement != nullptr)
	{
		int gid = tileElement->IntAttribute("gid");

		if (gid != 0)
			AddTileToLayer(layer, x, y, gid);

		// check if layer is testable to the pathfinder
		if (layer.properties.find("walkable") != layer.properties.end())
		{
			bool walkable = true;
			if (layer.properties["walkable"] == "true")
			{
				if (gid != 0)
				{
					if (nodes[y * m_width + x].walkable)
						walkable = true;
					else
						walkable = false;
				}
				else
				{
					if (nodes[y * m_width + x].walkable == false)
						walkable = false;
					if (layer.name == "background")
					{
						walkable = false;
					}
				}
			}
			else if(layer.properties["walkable"] == "false")
			{
				walkable = false;
				if (gid != 0)
				{
					if (nodes[y * m_width + x].walkable == true)
					{
						walkable = false;
					}
				}
				else
				{
					if (nodes[y * m_width + x].walkable == true)
						walkable = true;
				}
			}
			else
			{
				std::cout << "ERROR" << std::endl;
			}
			nodes[y * m_width + x].walkable = walkable;

		}
		tileElement = tileElement->NextSiblingElement("tile");
		x++;
		if (x == m_width)
		{
			x = 0;
			y++;
		}
	}

	m_layers.push_back(layer);
	return true;
}
void Maploader::ParseLayerProperties(tinyxml2::XMLElement* propertiesNode, MapLayer& layer)
{
	tinyxml2::XMLElement* propertyNode = propertiesNode->FirstChildElement("property");
	while (propertyNode != nullptr)
	{
		std::string name = propertyNode->Attribute("name");
		std::string value = propertyNode->Attribute("value");
		layer.properties[name] = value;
		propertyNode = propertyNode->NextSiblingElement("property");
		std::cout << "Added layer property " << name << " with value " << value << std::endl;
	}
}
bool Maploader::ParseObjectGroup(tinyxml2::XMLElement* objectGroupNode)
{
	std::cout << "Found objectgroup " << objectGroupNode->Attribute("name") << std::endl;

	ObjectGroup *group = new ObjectGroup();
	group->name = objectGroupNode->Attribute("name");
	group->width = objectGroupNode->IntAttribute("width");
	group->height = objectGroupNode->IntAttribute("height");
	group->objects.clear();
	tinyxml2::XMLElement* object = objectGroupNode->FirstChildElement("object");
	if (object == nullptr)
	{
		std::cout << "No objects found in " << group->name << std::endl;
	}
	
	while (object != nullptr)
	{
		// General attributes for all objects
		std::string name = "";
		std::string type = "";
		float x = 0.f;
		float y = 0.f;
		float width = 0.f;
		float height = 0.f;
		std::map<std::string, std::string> properties;

		// Fetch name and type of object
		if (object->Attribute("name") != nullptr)
			name = object->Attribute("name");
		else
			name = "";
		if (object->Attribute("type") != nullptr)
			type = object->Attribute("type");
		else
			type = "";

		// Get position of object
		if (object->Attribute("x") != nullptr && object->Attribute("y") != nullptr)
		{
			x = object->FloatAttribute("x");
			y = object->FloatAttribute("y");
		}

		// Get dimensions of object
		if (object->Attribute("width") != nullptr && object->Attribute("height") != nullptr)
		{
			width = object->FloatAttribute("width");
			height = object->FloatAttribute("height");
		}

		// Fetch properties for object
		if (tinyxml2::XMLElement* propertiesNode = object->FirstChildElement("properties")) {
			tinyxml2::XMLElement* propertyNode = propertiesNode->FirstChildElement("property");
			while (propertyNode != nullptr)
			{
				std::string name = propertyNode->Attribute("name");
				std::string value = propertyNode->Attribute("value");
				properties[name] = value;
				propertyNode = propertyNode->NextSiblingElement("property");
			}
		}

		// Find out type
		if (HasElement(object, "polyline") != nullptr)
		{
			tinyxml2::XMLElement* element = HasElement(object, "polyline");

			MapObjectPolyline *o = new MapObjectPolyline();
			o->name = name;
			o->type = type;
			o->x = x;
			o->y = y;
			o->width = width;
			o->height = height;
			o->m_properties = properties;
			o->points.clear();

			std::vector<std::string> points = String::explode(std::string(element->Attribute("points")), " ");
			if (!points.empty())
			{
				for (auto &point : points)
				{
					std::vector<std::string> split_points = String::explode(point, ",");
					sf::Vector2f p;
					p.x = x + static_cast<float>(std::atof(split_points.at(0).c_str()));
					p.y = y + static_cast<float>(std::atof(split_points.at(1).c_str()));
					o->points.push_back(p);
				}
			}
			o->mapObjectType = MAPOBJECT_POLYLINE;
			group->objects.push_back(o);
		}
		else
		{
			MapObjectRectangle *o = new MapObjectRectangle();
			o->name = name;
			o->type = type;
			o->x = x;
			o->y = y;
			o->width = width;
			o->height = height;
			o->mapObjectType = MAPOBJECT_RECTANGLE;
			o->m_properties = properties;
			group->objects.push_back(o);
		}
		object = object->NextSiblingElement("object");
	}
	m_objectgroups.push_back(group);
	return true;
}
tinyxml2::XMLElement* Maploader::HasElement(tinyxml2::XMLNode* parent, std::string identifier)
{
	if (parent != nullptr)
	{
		tinyxml2::XMLNode* child = parent->ToElement()->FirstChild();
		while (child != nullptr)
		{
			tinyxml2::XMLElement* childEl = child->ToElement();
			std::string real_name = std::string(childEl->Name());
			bool match = (real_name == identifier);
			if (match)
				return child->ToElement();
			child = child->NextSiblingElement();
		}
	}
	return nullptr;
}
TileQuad Maploader::AddTileToLayer(MapLayer& layer, int x, int y, int gid)
{
	int opacity = static_cast<int>(255.f * layer.opacity);
	sf::Color color = sf::Color(255, 255, 255, opacity);

	sf::Vertex v0;
	sf::Vertex v1;
	sf::Vertex v2;
	sf::Vertex v3;

	v0.texCoords = m_tileInfo[gid].Coords[0] + sf::Vector2f(0.5f, 0.5f);
	v1.texCoords = m_tileInfo[gid].Coords[1] + sf::Vector2f(-0.5f, 0.5f);
	v2.texCoords = m_tileInfo[gid].Coords[2] + sf::Vector2f(-0.5f, -0.5f);
	v3.texCoords = m_tileInfo[gid].Coords[3] + sf::Vector2f(0.5f, -0.5f);

	v0.position = sf::Vector2f(static_cast<float>(m_tileWidth * x), static_cast<float>(m_tileHeight * y));
	v1.position = sf::Vector2f(static_cast<float>(m_tileWidth * x) + m_tileInfo[gid].Size.x, static_cast<float>(m_tileHeight * y));
	v2.position = sf::Vector2f(static_cast<float>(m_tileWidth * x) + m_tileInfo[gid].Size.x, static_cast<float>(m_tileHeight * y) + m_tileInfo[gid].Size.y);
	v3.position = sf::Vector2f(static_cast<float>(m_tileWidth * x), static_cast<float>(m_tileHeight * y) + m_tileInfo[gid].Size.y);

	int tileHeight = static_cast<int>(m_tileInfo[gid].Size.y);
	if (tileHeight != m_tileHeight)
	{
		float diff = static_cast<float>(m_tileHeight - tileHeight);
		v0.position.y += diff;
		v1.position.y += diff;
		v2.position.y += diff;
		v3.position.y += diff;
	}

	v0.color = color;
	v1.color = color;
	v2.color = color;
	v3.color = color;

	int id = m_tileInfo[gid].TileSetId;
	if (layer.layerSets.find(id) == layer.layerSets.end())
	{
		layer.layerSets[id] = LayerSet(m_tilesetTextures[id]);
	}
	return layer.layerSets[id].AddTile(v0, v1, v2, v3);
}
void Maploader::Unload()
{
	m_tilesetTextures.clear();
	m_layers.clear();
	m_objectgroups.clear();
	m_mapLoaded = false;
}
void Maploader::Draw(sf::RenderTarget& rt, int draw_type)
{
	for (unsigned int i = 0; i < m_layers.size(); i++)
	{
		if (draw_type == MAP_DRAW_BLOCKING)
		{
			if (m_layers[i].hasProperty("blocking"))
			{
				rt.draw(m_layers[i]);
			}
		}
		else
		{
			rt.draw(m_layers[i]);
		}
	}
}

Maploader::TileInfo::TileInfo(): TileSetId(0u)
{

}
Maploader::TileInfo::TileInfo(const sf::IntRect rect, const sf::Vector2f size, int tilesetId): Size(size), TileSetId(tilesetId)
{
	Coords[0] = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top));
	Coords[1] = sf::Vector2f(static_cast<float>(rect.left + rect.width), static_cast<float>(rect.top));
	Coords[2] = sf::Vector2f(static_cast<float>(rect.left + rect.width), static_cast<float>(rect.top + rect.height));
	Coords[3] = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top + rect.height));
}
std::vector<sf::FloatRect> Maploader::getBoundsByGroup(std::string value)
{
	std::vector<sf::FloatRect> bounds;
	auto it = m_objectgroups.begin();
	while (it != m_objectgroups.end())
	{
		if ((*it)->name == value)
		{
			auto o_it = (*it)->objects.begin();
			while (o_it != (*it)->objects.end())
			{
				sf::FloatRect bound;
				bound.left = (*o_it)->x;
				bound.top = (*o_it)->y;
				bound.height = (*o_it)->height;
				bound.width = (*o_it)->width;
				bounds.push_back(bound);
				++o_it;
			}
			break;
		}
		++it;
	}

	return bounds;
}
void Maploader::retrieveSegments(LightEngine* lightEngine, std::string object_group)
{
	auto it = m_objectgroups.begin();
	int count = 0;
	while (it != m_objectgroups.end())
	{
		if ((*it)->name == object_group)
		{
			std::vector<PrepareSegment> segments;
			segments.clear();

			auto o_it = (*it)->objects.begin();
			while (o_it != (*it)->objects.end())
			{
				count++;
				MapObjectPolyline *mapObjectPolyline = dynamic_cast<MapObjectPolyline*>((*o_it));
				for (unsigned int i = 1; i < mapObjectPolyline->points.size(); i++)
				{
					PrepareSegment* s = new PrepareSegment();
					s->a = mapObjectPolyline->points[i-1];
					int nextPointIndex = i;
					s->b = mapObjectPolyline->points[nextPointIndex];
					lightEngine->m_prep_segments.push_back(s);
				}
				++o_it;
			}
		}
		++it;
	}
	std::cout << "Processed a total of " << count << " segments" << std::endl;
}
ObjectGroup* Maploader::getGroup(std::string identifier)
{
	auto it = m_objectgroups.begin();
	while (it != m_objectgroups.end())
	{
		if ((*it)->name == identifier)
		{
			return *it;
		}
		++it;
	}
	return nullptr;
}
void Maploader::drawDebug(sWindow* window)
{
	for (auto &group : m_objectgroups)
	{
		for (auto &obj : group->objects)
		{
			sf::RectangleShape shape;
			shape.setPosition(obj->x, obj->y);
			shape.setSize(sf::Vector2f(obj->width, obj->height));
			shape.setOutlineThickness(1);
			shape.setOutlineColor(sf::Color(255, 0, 0));
			window->draw(shape);
		}
	}
}

bool Maploader::hasProperty(std::string p_key)
{
	auto it = m_properties.find(p_key);
	if (it != m_properties.end())
	{
		return true;
	}
	return false;
}

std::string Maploader::getProperty(std::string p_key)
{
	auto it = m_properties.find(p_key);
	if (it != m_properties.end())
	{
		return it->second;
	}
	return "";
}