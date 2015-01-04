#pragma once

#include <array>
#include <vector>

#include "SFML\Graphics.hpp"
#include "MapLayer.h"
#include "ObjectGroup.h"
#include "tinyxml2.h"
#include "Window.h"
#include "Pathfinder.h"

class LightEngine;
struct PrepareSegment;

enum Map_Draw_States
{
	MAP_DRAW_NORMAL,
	MAP_DRAW_BLOCKING
};

class Maploader
{
public:
	Maploader(std::string mapDirectory);
	~Maploader();

	bool Load(Pathfinder* pathfinder);
	void AddSearchPath(std::string path);
	bool ParseMap();
	bool ParseTilesets();
	bool ProcessTiles(tinyxml2::XMLElement* tilesetNode);
	bool ParseLayer();
	bool ParseLayer(tinyxml2::XMLElement* layerNode, std::vector<Node> &nodes);
	void ParseLayerProperties(tinyxml2::XMLElement* propertiesNode, MapLayer &layer);
	bool ParseObjectGroup(tinyxml2::XMLElement* objectGroupNode);
	tinyxml2::XMLElement* HasElement(tinyxml2::XMLNode* parent, std::string identifier);
	TileQuad AddTileToLayer(MapLayer &layer, int x, int y, int gid);
	void Unload();
	void drawDebug(sWindow* window);
	void Draw(sf::RenderTarget& rt, int draw_type);
	tinyxml2::XMLElement* mapNode;

	std::vector<sf::FloatRect> getBoundsByGroup(std::string value);
	void retrieveSegments(LightEngine* lightEngine, std::string object_group);

	ObjectGroup* getGroup(std::string identifier);

	int m_width;
	int m_height;
	int m_tileWidth;
	int m_tileHeight;
	sf::FloatRect m_screenBounds;

	bool hasProperty(std::string p_key);
	std::string getProperty(std::string p_key);
private:
	std::map<std::string, std::string> m_properties;
	sf::FloatRect m_bounds;
	mutable sf::Vector2f m_lastViewPos;

	std::vector<MapLayer> m_layers;
	std::vector<ObjectGroup*> m_objectgroups;
	std::vector<sf::Texture*> m_tilesetTextures;
	std::vector<std::vector<Node>> m_walkableNodes;
	struct TileInfo
	{
		std::array<sf::Vector2f, 4> Coords;
		sf::Vector2f Size;
		int TileSetId;
		TileInfo();
		TileInfo(sf::IntRect rect, sf::Vector2f size, int tilesetId);
	};
	std::vector<TileInfo> m_tileInfo;

	std::string m_mapPath;
	bool m_mapLoaded;
};

