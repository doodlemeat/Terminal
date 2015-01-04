#include "Window.h"

std::map<std::string, sWindow*> Window::windows;

Window::Window()
{
}


Window::~Window()
{
}

void Window::create(std::string name, std::string title, unsigned int width, unsigned int height)
{
	sWindow* window = new sWindow();
	window->m_fullscreen = true;
	window->m_verticalSync = true;
	window->create(sf::VideoMode(width, height), title, sf::Style::Fullscreen);
	window->setVerticalSyncEnabled(window->m_verticalSync);
	window->title = title;
	window->width = width;
	window->height = height;
	windows.insert(std::pair<std::string, sWindow*>(name, window));
}

sWindow* Window::get(std::string name)
{
	auto it = windows.find(name);
	if (it != windows.end())
	{
		return it->second;
	}
	return nullptr;
}

void Window::cleanup()
{
	auto it = windows.begin();
	while (it != windows.end())
	{
		if (it->second->isOpen())
		{
			it->second->close();
		}
		delete it->second;
		it++;
	}
}

void Window::isFullscreen()
{

}

void sWindow::changeResolution(sf::Vector2i resolution)
{
	if (width != resolution.x || height != resolution.y)
	{
		width = resolution.x;
		height = resolution.y;
		reCreate();
	}
}

void sWindow::reCreate()
{
	create(sf::VideoMode(width, height), title, sf::Style::Fullscreen);
	setVerticalSyncEnabled(m_verticalSync);
}
sf::Vector2f sWindow::getMousePosition()
{
	return mapPixelToCoords(sf::Mouse::getPosition());
}

sf::Vector2f sWindow::getMousePositionWindow()
{
	return mapPixelToCoords(sf::Mouse::getPosition(*this));
}

