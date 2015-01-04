#include "boost\math\special_functions\fpclassify.hpp"
#include "State.h"
#include "Window.h"
#include "Input.h"
#include "MenuState.h"
#include "Audiomanager.h"
#include "SpriteManager.h"
#include "SFML/Graphics/Sprite.hpp"
#include "Button.h"
#include "Thor/Input.hpp"
#include "Audiomanager.h"
#include <iostream>
#include "Font.h"
#include "boost/lexical_cast.hpp"
#include "Config.h"

namespace
{
	sf::Vector2f scalePosition(sf::Vector2f position, sf::Vector2f scale)
	{
		return sf::Vector2f(position.x * scale.x, position.y * scale.y);
	}
}
MenuState::MenuState() 
{
	m_spriteManager = nullptr;
	m_startBtn = nullptr;
	m_optionsBtn = nullptr;
	m_creditsBtn = nullptr;
	m_exitBtn = nullptr;
	m_background = nullptr;
	m_buttonsBackground = nullptr;
	m_logo = nullptr;
}
bool MenuState::Init() 
{
	std::cout << "Entered menu" << std::endl;
	m_toggleOptions = false;
	m_optionsView = OPTION_AUDIO;
	

	m_spriteManager = new SpriteManager();
	sf::Vector2u windowSize = Window::get("main")->getSize();

	/* background image */
	m_background = new sf::Sprite();
	m_background->setPosition(0, 0);
	m_background->setTexture(*m_spriteManager->loadImage("menu_bg.png"));
	m_background->setScale(windowSize.x / m_background->getGlobalBounds().width, windowSize.y / m_background->getGlobalBounds().height);

	/* buttons background image */
	m_buttonsBackground = new sf::Sprite();
	m_buttonsBackgroundDefaultPosition = sf::Vector2f(60, 270);
	m_buttonsBackground->setPosition(m_buttonsBackgroundDefaultPosition);
	m_buttonsBackground->setTexture(*m_spriteManager->loadImage("gui/menu_frame.png"));

	/* Options background */
	m_optionsBackground = new sf::Sprite();
	m_optionsBackgroundDefaultPosition = sf::Vector2f(690, 378);
	m_optionsBackground->setPosition(m_optionsBackgroundDefaultPosition);
	m_optionsBackground->setTexture(*m_spriteManager->loadImage("gui/optionsbg.png"));

	/* logo */
	m_logo = new sf::Sprite();
	m_logo->setTexture(*m_spriteManager->loadImage("gui/game_logo.png"));
	m_logoYOffset = 30;
	m_logo->setPosition(windowSize.x / 2, m_logoYOffset);
	m_logo->setOrigin(m_logo->getGlobalBounds().width / 2, 0);

	m_startBtn = new Button();
	m_optionsBtn = new Button();
	m_creditsBtn = new Button();
	m_exitBtn = new Button();

	/* set buttons size */
	m_startBtn->setSize(637, 143);
	m_optionsBtn->setSize(637, 143);
	m_creditsBtn->setSize(637, 143);
	m_exitBtn->setSize(637, 143);

	/* set button images */
	m_startBtn->getSprite()->setTexture(*m_spriteManager->loadImage("gui/start_p.png"));
	m_optionsBtn->getSprite()->setTexture(*m_spriteManager->loadImage("gui/options_p.png"));
	m_creditsBtn->getSprite()->setTexture(*m_spriteManager->loadImage("gui/credits_p.png"));
	m_exitBtn->getSprite()->setTexture(*m_spriteManager->loadImage("gui/exit_p.png"));

	/* setup button callbacks */
	m_startBtn->addCallback(DOWN, std::bind(&MenuState::handleStartButtonDown, this));
	m_optionsBtn->addCallback(DOWN, std::bind(&MenuState::handleOptionsButtonDown, this));
	m_creditsBtn->addCallback(DOWN, std::bind(&MenuState::handleCreditsButtonDown, this));
	m_exitBtn->addCallback(DOWN, std::bind(&MenuState::handleExitButtonDown, this));

	m_startBtnDefaultPosition = sf::Vector2f(50, 320);
	m_optionsBtnDefaultPosition = sf::Vector2f(50, 473);
	m_creditsBtnDefaultPosition = sf::Vector2f(50, 616);
	m_exitBtnDefaultPosition = sf::Vector2f(50, 759);

	/* set button position */
	m_startBtn->getSprite()->setPosition(m_startBtnDefaultPosition);
	m_optionsBtn->getSprite()->setPosition(m_optionsBtnDefaultPosition);
	m_creditsBtn->getSprite()->setPosition(m_creditsBtnDefaultPosition);
	m_exitBtn->getSprite()->setPosition(m_exitBtnDefaultPosition);

	/* Button mute */
	m_optionsMute = new Button();
	m_optionsMute->setSize(64, 64);
	m_optionsMuteDefaultPosition = sf::Vector2f(100, 100);
	m_optionsMute->getSprite()->setPosition(m_optionsBackground->getPosition() + m_optionsMuteDefaultPosition);
	m_optionsMute->getSprite()->setTexture(*m_spriteManager->loadImage("gui/checkbox.png"));
	m_optionsMute->setToggleable(true);
	m_optionsMute->addCallback(DOWN, std::bind(&MenuState::handleMuteButton, this));

	/* Button fullscreen */
	m_optionsFullscreen = new Button();
	m_optionsFullscreen->setSize(64, 64);
	m_optionsFullscreenDefaultPosition = sf::Vector2f(100, 100);
	m_optionsFullscreen->getSprite()->setPosition(m_optionsBackground->getPosition() + m_optionsFullscreenDefaultPosition);
	m_optionsFullscreen->getSprite()->setTexture(*m_spriteManager->loadImage("gui/checkbox.png"));
	m_optionsFullscreen->setToggleable(true);
	m_optionsFullscreen->addCallback(DOWN, std::bind(&MenuState::handleFullscreenButton, this));

	/* Option buttons */
	m_buttonList = new ButtonList();
	{
		Button* button = new Button();
		button->setSize(119, 44);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("hud/audio.png"));
		button->setID(OPTIONS_BUTTON_AUDIO);
		m_buttonList->addButton(button);
	}
	{
		Button* button = new Button();
		button->setSize(187, 44);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("hud/graphics.png"));
		button->setID(OPTIONS_BUTTON_GRAPHICS);
		m_buttonList->addButton(button);
	}
	{
		Button* button = new Button();
		button->setSize(100, 30);
		button->setToggleable(true);
		button->getSprite()->setTexture(*SpriteManager::loadImage("buttons/controls.png"));
		button->setID(OPTIONS_BUTTON_CONTROLS);
		m_buttonList->addButton(button);
	}
	m_optionLinksDefaultPosition = sf::Vector2f(720, 408);
	m_buttonList->setPosition(m_optionLinksDefaultPosition);
	m_buttonList->toggleButton(0);
	m_buttonList->setCallbackHandler(std::bind(&MenuState::changeOptionsState, this, std::placeholders::_1, std::placeholders::_2));

	m_optionViewport = new sf::View();
	m_optionViewport->setSize(407, 385);
	m_optionViewport->setCenter(924, 639);

	float center_x = m_optionViewport->getCenter().x;
	float center_y = m_optionViewport->getCenter().y;

	float left_x = center_x - (m_optionViewport->getSize().x / 2);
	float left_y = center_y - (m_optionViewport->getSize().y / 2);
	float right_x = center_x + (m_optionViewport->getSize().x / 2);
	float right_y = center_y + (m_optionViewport->getSize().y / 2);

	float viewport_x = left_x / windowSize.x;
	float viewport_y = left_y / windowSize.y;
	float viewport_w = right_x / windowSize.x;
	float viewport_h = right_y / windowSize.y;

	viewport_w -= viewport_x;
	viewport_h -= viewport_y;

	sf::FloatRect viewport(viewport_x, viewport_y, viewport_w, viewport_h);
	m_optionViewport->setViewport(viewport);
	return true;
}
void MenuState::Exit() 
{
	delete m_background;
	delete m_buttonsBackground;
	delete m_logo;
	delete m_optionsBackground;
	delete m_optionsMute;
	delete m_optionsFullscreen;
	delete m_buttonList;
	delete m_startBtn;
	delete m_optionsBtn;
	delete m_exitBtn;
	delete m_spriteManager;
	delete m_optionViewport;

	m_background = nullptr;
	m_buttonsBackground = nullptr;
	m_logo = nullptr;
	m_optionsBackground = nullptr;
	m_optionsMute = nullptr;
	m_optionsFullscreen = nullptr;
	m_buttonList = nullptr;
	m_startBtn = nullptr;
	m_optionsBtn = nullptr;
	m_exitBtn = nullptr;
	m_spriteManager = nullptr;
	m_optionViewport = nullptr;
}
bool MenuState::Update() 
{
	if (m_toggleOptions)
	{
		float viewport_top = m_optionViewport->getCenter().y - m_optionViewport->getSize().y / 2;
		float viewport_bottom = m_optionViewport->getCenter().y + m_optionViewport->getSize().y / 2;

		switch (m_optionsView)
		{
		case OPTION_AUDIO:
			m_optionsMute->update();
			break;
		case OPTION_GRAPHICS:
			m_optionsFullscreen->update();
			if (Input::pressed("mouse_scroll_down"))
			{
				
			}
			if (Input::pressed("mouse_scroll_up"))
			{
				
			}
			break;
		case OPTION_CONTROLS:
			break;
		}
	}
	m_startBtn->update();
	m_optionsBtn->update();
	m_creditsBtn->update();
	m_exitBtn->update();
	m_buttonList->update();


	if (Next() == "Quit")
	{
		setNextState("");
		return false;
	}
	if (!Next().empty())
	{
		return false;
	}

	return true;
}
void MenuState::Draw()
{
	Window::get("main")->draw(*m_background);
	Window::get("main")->draw(*m_buttonsBackground);
	Window::get("main")->draw(*m_startBtn);
	Window::get("main")->draw(*m_logo);
	Window::get("main")->draw(*m_optionsBtn);
	Window::get("main")->draw(*m_creditsBtn);
	Window::get("main")->draw(*m_exitBtn);

	if (m_toggleOptions)
	{
		Window::get("main")->draw(*m_optionsBackground);
		Window::get("main")->draw(*m_buttonList);
		Window::get("main")->setView(*m_optionViewport);
		switch (m_optionsView)
		{
		case OPTION_AUDIO:
			drawAudio();
			break;
		case OPTION_GRAPHICS:
			drawGraphics();
			break;
		case OPTION_CONTROLS:
			drawControls();
			break;
		}
	}
	Window::get("main")->setView(Window::get("main")->getDefaultView());
}
std::string MenuState::Next()
{
	return next_state;
}
void MenuState::setNextState(std::string state)
{
	next_state = state;
}
bool MenuState::IsType(const std::string &type)
{
	return (type == "MenuState");
}
void MenuState::handleStartButtonDown()
{
	setNextState("GameState");
}	
void MenuState::handleOptionsButtonDown()
{
	m_toggleOptions = !m_toggleOptions;
}
void MenuState::handleCreditsButtonDown()
{
	setNextState("OptionsState");
}
void MenuState::handleExitButtonDown()
{
	setNextState("Quit");
}
void MenuState::changeOptionsState(int p_buttonID, int p_state)
{
	std::cout << "State: " << p_state << std::endl;
	if (p_state == DOWN)
	{
		switch (p_buttonID)
		{
		case OPTIONS_BUTTON_AUDIO:
			m_optionsView = OPTION_AUDIO;
			break;
		case OPTIONS_BUTTON_GRAPHICS:
			m_optionsView = OPTION_GRAPHICS;
			break;
		case OPTIONS_BUTTON_CONTROLS:
			m_optionsView = OPTION_CONTROLS;
			break;
		}
	}
}
void MenuState::drawAudio()
{
	Window::get("main")->draw(*m_optionsMute);
}
void MenuState::drawGraphics()
{
	Window::get("main")->draw(*m_optionsFullscreen);
}
void MenuState::drawControls()
{
}
void MenuState::handleMuteButton()
{
	if (Audiomanager::isMuted())
	{
		Audiomanager::unmuteAll(100);
	}
	else
	{
		Audiomanager::muteAll();
	}
}
void MenuState::handleFullscreenButton()
{

}
