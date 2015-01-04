#include "Pathfinder.h"
#include "SFML\Graphics\CircleShape.hpp"
#include <iostream>


sf::Vector2i Node::getPosition()
{
	return sf::Vector2i(this->x, this->y);
}
int Node::getTileId()
{
	return y * width + x;
}

Pathfinder::Pathfinder()
{

}
Pathfinder::~Pathfinder()
{

}
void Pathfinder::retrieveNodes(std::vector<Node> nodes, int width, int height, int tileWidth, int tileHeight)
{
	std::cout << "[PATHFINDER] Nodes retrieved!" << std::endl;
	this->m_width = width;
	this->m_height = height;
	this->m_tileWidth = tileWidth;
	this->m_tileHeight = tileHeight;
	this->m_nodes = nodes;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		m_nodes[i].width = this->m_width;
		m_nodes[i].height = this->m_height;

		if (m_nodes[i].walkable)
		{
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(32, 32));
			rect.setPosition(32.f * m_nodes[i].x, 32.f * m_nodes[i].y);
			rect.setFillColor(sf::Color(255, 0, 0, 150));
			m_nodesShape.push_back(rect);
		}
	}
	this->m_nodesBackup = m_nodes;
}
void Pathfinder::setStartAndGoal(sf::Vector2i start, sf::Vector2i goal)
{
	this->m_start = start.y * m_width + start.x;
	this->m_goal = goal.y * m_width + goal.x;
	if (!this->m_nodes[m_start].walkable || !this->m_nodes[m_goal].walkable)
	{
		std::cout << "Start and goal must be walkable" << std::endl;
	}
	this->m_goalPosition = goal;
	this->m_startPosition = start;
}
std::vector<sf::Vector2i> Pathfinder::execute()
{
	this->m_openList.push_back(this->m_start);

	// while the openlist is not empty
	std::cout << "[PATHFINDER] Finding a path from " << m_start << " to " << m_goal << std::endl;
	while (!this->m_openList.empty())
	{
		// Find the node with lowest f value in openlist
		int lowestIndex = 0;
		for (unsigned int i = 0; i < this->m_openList.size(); i++)
		{
			if (this->m_nodes[i].f < this->m_nodes[lowestIndex].f)
			{
				lowestIndex = i;
			}
		}
		Node* currentNode = &this->m_nodes[this->m_openList[lowestIndex]];

		// Check if we reached the goal
		if (currentNode->getTileId() == m_goal)
		{
			m_closedList.push_back(currentNode->getTileId());
			int startIndex = m_closedList.back();
			Node* parent = &m_nodes[startIndex];
			while (parent != nullptr)
			{
				m_results.push_back(sf::Vector2i(parent->x, parent->y));
				parent = parent->parent;
			}
			std::reverse(m_results.begin(), m_results.end());
			m_openList.clear();
		}
		else // not the destination
		{

			// Move currentNode from open to closed
			this->m_openList.erase(this->m_openList.begin() + lowestIndex);
			currentNode->closed = true;

			// Process each of its neighbours
			std::vector<int> neighbors = this->findNeighbors(currentNode, m_allowDiagonalMovement, m_dontCrossCorners);
			for (unsigned int i = 0; i < neighbors.size(); i++)
			{
				Node* currentNeighbor = &m_nodes[neighbors[i]];
				if (currentNeighbor->closed)
				{
					// Not a valid node to process, skip to next neighbor
					continue;
				}

				currentNeighbor->parent = currentNode;

				// Fetch the G value of this node which is the current nodes G value plus the value from this current node to this neighbor node
				currentNeighbor->g = currentNode->g + this->ManhattanDistance(currentNeighbor->getPosition(), currentNode->getPosition());
				currentNeighbor->f = currentNeighbor->g + this->ManhattanDistance(currentNeighbor->getPosition(), m_goalPosition);
				currentNeighbor->closed = true;
				m_openList.push_back(neighbors[i]);
			}

			m_closedList.push_back(currentNode->getTileId());
		}
	}
	if (m_results.size() == 0)
	{
		std::cout << "[PATHFINDER] Seems like a path couldn't be found. Draw the pathfinder and check!" << std::endl;
	}
	return this->m_results;
}
std::vector<int> Pathfinder::findNeighbors(Node* currentNode, bool allowDiagonal, bool dontCrossCorners)
{
	std::vector<int> neighbors(0, 0);
	int x = currentNode->x;
	int y = currentNode->y;
	int max = m_nodes.size();

	bool c0 = false, c1 = false, c2 = false, c3 = false;
	bool n0 = false, n1 = false, n2 = false, n3 = false;

	int northNodeIndex = (y - 1) * m_width + x;  // ↑
	int southNodeIndex = (y + 1) * m_width + x;  // ↓
	int westNodeIndex = y * m_width + (x - 1);   // ←
	int eastNodeIndex = y * m_width + (x + 1);   // →

	if (isWalkable(northNodeIndex))
	{
		neighbors.push_back(northNodeIndex);
		n0 = true;
	}
	if (isWalkable(southNodeIndex))
	{
		neighbors.push_back(southNodeIndex);
		n1 = true;
	}
	if (isWalkable(westNodeIndex))
	{
		neighbors.push_back(westNodeIndex);
		n2 = true;
	}
	if (isWalkable(eastNodeIndex))
	{
		neighbors.push_back(eastNodeIndex);
		n3 = true;
	}
	
	if (!allowDiagonal)
	{
		return neighbors;
	}

	if (dontCrossCorners)
	{
		c0 = n2 && n0; // ↖
		c1 = n3 && n0; // ↗
		c2 = n2 && n1; // ↘
		c3 = n3 && n1; // ↙
	}
	else
	{
		c0 = n2 || n0; // ↖
		c1 = n3 || n0; // ↗
		c2 = n2 || n1; // ↘
		c3 = n3 || n1; // ↙
	}

	/* Process diagonal neighbors */
	int northWestNodeIndex = (y - 1) * m_width + (x - 1); // ↖
	int northEastNodeIndex = (y - 1) * m_width + (x + 1); // ↗
	int southWestNodeIndex = (y + 1) * m_width + (x - 1); // ↙
	int southEastNodeIndex = (y + 1) * m_width + (x + 1); // ↘

	if (isWalkable(northWestNodeIndex) && c0) neighbors.push_back(northWestNodeIndex);
	if (isWalkable(northEastNodeIndex) && c1) neighbors.push_back(northEastNodeIndex);
	if (isWalkable(southWestNodeIndex) && c2) neighbors.push_back(southWestNodeIndex);
	if (isWalkable(southEastNodeIndex) && c3) neighbors.push_back(southEastNodeIndex);

	return neighbors;
}
void Pathfinder::reset()
{
	this->m_start = 0;
	this->m_goal = 0;
	this->m_openList.clear();
	this->m_closedList.clear();
	this->m_results.clear();
	this->m_nodes = this->m_nodesBackup;
	this->m_allowDiagonalMovement = false;
	this->m_dontCrossCorners = false;
}
void Pathfinder::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (sf::RectangleShape shape : m_nodesShape)
	{
		target.draw(shape);
	}
	sf::RectangleShape start;
	start.setSize(sf::Vector2f(30, 30));
	start.setFillColor(sf::Color::Cyan);
	start.setPosition(static_cast<int>(m_startPosition.x) * static_cast<int>(m_tileWidth), static_cast<int>(m_startPosition.y) * static_cast<int>(m_tileHeight));
	target.draw(start);

	sf::RectangleShape goal;
	goal.setSize(sf::Vector2f(30, 30));
	goal.setFillColor(sf::Color::Blue);
	goal.setPosition(static_cast<int>(m_goalPosition.x) * static_cast<int>(m_tileWidth), static_cast<int>(m_goalPosition.y) * static_cast<int>(m_tileHeight));
	target.draw(goal);
}
int Pathfinder::ManhattanDistance(sf::Vector2i point, sf::Vector2i goal)
{
	return static_cast<int>(abs(point.x - goal.x) + abs(point.y - goal.y));
}

sf::Vector2f Pathfinder::getTileSize()
{
	return sf::Vector2f(static_cast<int>(m_tileWidth), static_cast<int>(m_tileHeight));
}
sf::Vector2i Pathfinder::getTilePosition(sf::Vector2f pos)
{
	int y = static_cast<int>(pos.y) / m_tileHeight;
	int x = static_cast<int>(pos.x) / m_tileWidth;
	return sf::Vector2i(x, y);
}


bool Pathfinder::isWalkable(unsigned int p_nodeID)
{
	return (p_nodeID >= 0 && p_nodeID <= m_nodes.size() && m_nodes[p_nodeID].walkable);
}

void Pathfinder::setAllowDiagonalMovement(bool p_value)
{
	m_allowDiagonalMovement = p_value;
}
void Pathfinder::setDontCrossCorners(bool p_value)
{
	m_dontCrossCorners = p_value;
}
