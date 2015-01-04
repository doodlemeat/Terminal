#pragma once

#include "State.h"
#include "Thor/Input.hpp"
#include "Button.h"

class SpriteManager;

namespace sf
{
	class Sprite;
	class Text;
	class View;
};

enum OptionButtonType
{
	OPTIONS_BUTTON_AUDIO,
	OPTIONS_BUTTON_GRAPHICS,
	OPTIONS_BUTTON_CONTROLS
};

enum OptionState
{
	OPTION_AUDIO,
	OPTION_GRAPHICS,
	OPTION_CONTROLS
};

class MenuState : public State {
public:
	MenuState();
	bool Init();
	void Exit();

	bool Update();
	void Draw();

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

	void handleStartButtonDown();
	void handleOptionsButtonDown();
	void handleCreditsButtonDown();
	void handleExitButtonDown();
	void changeOptionsState(int p_buttonID, int p_state);
	void drawAudio();
	void drawGraphics();
	void drawControls();
	void handleMuteButton();
	void handleFullscreenButton();
private:
	std::string next_state;

	Button* m_startBtn;
	Button* m_optionsBtn;
	Button* m_creditsBtn;
	Button* m_exitBtn;
	
	sf::Vector2f m_startBtnDefaultPosition;
	sf::Vector2f m_optionsBtnDefaultPosition;
	sf::Vector2f m_creditsBtnDefaultPosition;
	sf::Vector2f m_exitBtnDefaultPosition;
	
	ButtonList* m_buttonList;
	sf::Vector2f m_optionLinksDefaultPosition;

	SpriteManager* m_spriteManager;

	sf::Sprite* m_background;
	sf::Sprite* m_buttonsBackground;
	sf::Sprite* m_logo;
	sf::Sprite* m_optionsBackground;
	
	sf::Vector2f m_buttonsBackgroundDefaultPosition;
	sf::Vector2f m_optionsBackgroundDefaultPosition;
	float m_logoYOffset;

	/* OPTIONS STUFF */
	bool m_toggleOptions;
	OptionState m_optionsView;
	sf::View* m_optionViewport;
		/* AUDIO */
		Button* m_optionsMute;
		sf::Vector2f m_optionsMuteDefaultPosition;

		/* GRAPHICS */
		Button* m_optionsFullscreen;
		sf::Vector2f m_optionsFullscreenDefaultPosition;

		/* CONTROLS */
	/* --/ */

	thor::ActionMap<std::string> m_actionMap;
};

