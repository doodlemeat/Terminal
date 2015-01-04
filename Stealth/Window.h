#pragma once

#include <map>
#include "SFML\Graphics\RenderWindow.hpp"
#include "SFML\Window\Mouse.hpp"

struct sWindow: public sf::RenderWindow
{
	std::string title;
	unsigned int width;
	unsigned int height;
	sf::Time deltatime;
	bool m_fullscreen;
	bool m_verticalSync;

	using RenderWindow::draw;
	
	sf::Vector2f getMousePosition();
	sf::Vector2f getMousePositionWindow();
	void changeResolution(sf::Vector2i resolution);
	void reCreate();
};

class Window
{
public:
	Window();
	~Window();

	static void create(std::string name, std::string title, unsigned int width, unsigned int height);
	static sWindow* get(std::string name);
	static void cleanup();
	static void isFullscreen();
private:
	static std::map<std::string, sWindow*> windows;
};

