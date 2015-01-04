#include "MapLayer.h"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include <iostream>

TileQuad::TileQuad(int i0, int i1, int i2, int i3)
: m_needsUpdate(false)
{
	m_indices[0] = i0;
	m_indices[1] = i1;
	m_indices[2] = i2;
	m_indices[3] = i3;
}
void TileQuad::Move(const sf::Vector2f& distance)
{
	m_movement = distance;
	m_needsUpdate = true;
}

MapLayer::MapLayer(MapLayerType layerType)
{
}
MapLayer::~MapLayer()
{
}
bool MapLayer::hasProperty(std::string key)
{
	auto it = properties.find(key);
	if (it != properties.end())
	{
		return true;
	}
	return false;
}
void MapLayer::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	for (auto& ls : layerSets)
	{
		rt.draw(ls.second, states);
	}
}

LayerSet::LayerSet()
{

}
LayerSet::LayerSet(const sf::Texture* texture) : m_texture(texture)
{

}
TileQuad LayerSet::AddTile(sf::Vertex v0, sf::Vertex v1, sf::Vertex v2, sf::Vertex v3)
{
	m_vertices.push_back(v0);
	m_vertices.push_back(v1);
	m_vertices.push_back(v2);
	m_vertices.push_back(v3);

	int i = m_vertices.size() - 4;
	m_quads.push_back(TileQuad(i, i+1, i+2, i+3));
	return m_quads.back();
}
void LayerSet::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	for (auto &q : m_quads)
	{
		if (q.m_needsUpdate)
		{
			for (auto &i : q.m_indices)
			{
				m_vertices[i].position += q.m_movement;
			}
			q.m_needsUpdate = false;
		}
	}
	if (!m_vertices.empty())
	{
		states.texture = m_texture;
		rt.draw(&m_vertices[0], static_cast<unsigned int>(m_vertices.size()), sf::Quads, states);
	}
}