#pragma once
#include <map>
#include <vector>
#include <array>

#include "SFML\Graphics\Drawable.hpp"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include "SFML\Graphics\Texture.hpp"

class TileQuad
{
public:
	TileQuad(int i0, int i1, int i2, int i3);
	void Move(const sf::Vector2f &distance);
	bool m_needsUpdate;
	std::array<int, 4> m_indices;
	sf::Vector2f m_movement;
	int x;
	int y;
};

///////////////////////////////

class LayerSet final: public sf::Drawable
{
public:
	LayerSet();
	LayerSet(const sf::Texture* texture);
	TileQuad AddTile(sf::Vertex v0, sf::Vertex v1, sf::Vertex v2, sf::Vertex v3);

	const sf::Texture* m_texture;
	void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
	mutable std::vector<TileQuad> m_quads;
	mutable std::vector<sf::Vertex> m_vertices;
};


//////////////////////////////////

enum MapLayerType
{
	Layer,
	ImageLayer
};

class MapLayer final: public sf::Drawable
{
public:
	MapLayer(MapLayerType layerType);
	~MapLayer();

	bool hasProperty(std::string key);

	float opacity;
	std::string name;
	bool visible;
	std::map<std::string, std::string> properties;
	std::map<int, LayerSet> layerSets;

private:
	void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
};

