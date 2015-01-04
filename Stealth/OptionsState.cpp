#include "State.h"
#include "SFML\System\String.hpp"
#include "SFML\Graphics\Font.hpp"
#include "Text.h"
#include "Font.h"
#include "Window.h"
#include "Input.h"
#include "SpriteManager.h"
#include "OptionsState.h"
#include "Config.h"
#include "Button.h"
#include "boost\lexical_cast.hpp"

OptionsState::OptionsState()
{
	m_buttonList = nullptr;
}

bool OptionsState::Init()
{
	sf::Vector2u windowSize = Window::get("main")->getSize();
	m_background = new sf::RectangleShape();
	m_background->setFillColor(sf::Color::Yellow);
	m_background->setSize(
		sf::Vector2f(
		Config::root_node["general"]["gui_scale_x"].as<float>() / 100.f * Window::get("main")->getSize().x,
		Config::root_node["general"]["gui_scale_y"].as<float>() / 100.f * Window::get("main")->getSize().y
		)
	);
	m_background->setPosition(
		windowSize.x / 2 - m_background->getSize().x / 2, 
		windowSize.y / 2 - m_background->getSize().y / 2);
	/*std::vector<sf::VideoMode> myVideoModes = sf::VideoMode::getFullscreenModes();
	if (!myVideoModes.empty())
	{
	for (unsigned int i = 0; i < myVideoModes.size(); i++)
	{
	Resolution* res = new Resolution();
	res->x = myVideoModes[i].width;
	res->y = myVideoModes[i].height;
	sf::Text* text = new sf::Text();
	text->setFont(*Font::get("tahoma"));
	text->setString(boost::lexical_cast<std::string>(res->x) + "x" + boost::lexical_cast<std::string>(res->y));
	res->text = text;
	m_resolutions.push_back(res);
	}
	}
	m_currentResolution = 0;
	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	for (unsigned int i = 0; i < m_resolutions.size(); i++)
	{
	if (m_resolutions[i]->x == videoMode.width && m_resolutions[i]->y == videoMode.height)
	{
	m_currentResolution = i;
	}
	}*/

	m_buttonList = new ButtonList();
	{
		Button* button = new Button();
		button->setSize(100, 30);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("buttons/test_toggle.png"));
		button->setID(GAME_OPTIONS_BUTTON);
		m_buttonList->addButton(button);
	}
	{
		Button* button = new Button();
		button->setSize(100, 30);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("buttons/test_toggle.png"));
		button->setID(GRAPHICS_OPTIONS_BUTTON);
		m_buttonList->addButton(button);
	}
	{
		Button* button = new Button();
		button->setSize(100, 30);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("buttons/test_toggle.png"));
		button->setID(CONTROLS_OPTIONS_BUTTON);
		m_buttonList->addButton(button);
	}
	{
		Button* button = new Button();
		button->setSize(100, 30);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("buttons/test_toggle.png"));
		button->setID(AUDIO_OPTIONS_BUTTON);
		m_buttonList->addButton(button);
	}
	m_buttonList->setPosition(m_background->getPosition().x, m_background->getPosition().y - m_buttonList->getSize().y);
	m_buttonList->toggleButton(0);
	m_buttonList->setCallbackHandler(std::bind(&OptionsState::changeOptionsState, this, std::placeholders::_1, std::placeholders::_2));
	return true;
}
void OptionsState::Exit()
{
	/*for (unsigned int i = 0; i < m_resolutions.size(); i++)
	{
	delete m_resolutions[i]->text;
	delete m_resolutions[i];
	}
	m_resolutions.clear();
	*/
	delete m_buttonList;
	m_buttonList = nullptr;
}

bool OptionsState::Update()
{
	if (Input::pressed_once("a"))
	{
		setNextState("Game");
		return false;
	}
	m_buttonList->update();
	return true;
}

void OptionsState::Draw()
{
	Window::get("main")->draw(*m_buttonList);
	Window::get("main")->draw(*m_background);
}

std::string OptionsState::Next()
{
	return next_state;
}
void OptionsState::setNextState(std::string state)
{
	next_state = state;
}
bool OptionsState::IsType(const std::string &type)
{
	return (type == "OptionsState");
}

void OptionsState::changeOptionsState(int p_buttonID, int p_state)
{
	switch (p_buttonID)
	{
	case GAME_OPTIONS_BUTTON:

		break;
	case GRAPHICS_OPTIONS_BUTTON:
		
		break;
	case AUDIO_OPTIONS_BUTTON:

		break;
	case CONTROLS_OPTIONS_BUTTON:

		break;
	}
}