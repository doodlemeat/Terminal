#pragma once

#include "SFML\Graphics\Drawable.hpp"
#include "SFML\Graphics\RectangleShape.hpp"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include <vector>

// structure representing a node in the grid
struct Node
{
	Node* parent;
	int x;
	int y;
	int f;
	int g;
	int width;
	int height;
	bool closed;
	bool walkable;


	sf::Vector2i getPosition();
	int getTileId();
};

class Pathfinder: public sf::Drawable
{
public:
	Pathfinder();
	~Pathfinder();

	// This method retrieves nodes and put them in m_nodesBackup. Should only be run once.
	void retrieveNodes(std::vector<Node> nodes, int width, int height, int tileWidth, int tileHeight);

	// This should be called everytime before we need to find a path
	void setStartAndGoal(sf::Vector2i start, sf::Vector2i goal);

	// Returning a path to goal from start, if one is found... 
	std::vector<sf::Vector2i> execute();

	// This is called in the end of execute() to reset some thing like start and goal
	void reset();

	// Returns a vector of nodes which is the neighbors to currentNode
	std::vector<int> findNeighbors(Node* currentNode, bool allowDiagonal, bool dontCrossCorners);

	// For debugging. Draws rectangles where all nodes are
	virtual void draw(sf::RenderTarget &tg, sf::RenderStates states) const;

	// Calculate the manhattan distance
	int ManhattanDistance(sf::Vector2i point, sf::Vector2i goal);

	/* Returns tile width and tile height */
	sf::Vector2f getTileSize();

	/* Get a tiles position by a global position */
	sf::Vector2i getTilePosition(sf::Vector2f pos);

	bool isWalkable(unsigned int p_nodeID);

	void setAllowDiagonalMovement(bool p_value);
	void setDontCrossCorners(bool p_value);
private:
	std::vector<sf::RectangleShape> m_nodesShape;
	std::vector<Node> m_nodes;
	std::vector<Node> m_nodesBackup; 
	std::vector<int> m_openList;
	std::vector<int> m_closedList;
	std::vector<sf::Vector2i> m_results;
	
	sf::Vector2i m_goalPosition;
	sf::Vector2i m_startPosition;

	int m_start;
	int m_goal;
	int m_width;
	int m_height;
	int m_tileWidth;
	int m_tileHeight;
	
	bool m_dontCrossCorners;
	bool m_allowDiagonalMovement;
};

