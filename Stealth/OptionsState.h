#pragma once

#include "State.h"
#include <vector>

class ButtonList;

namespace sf
{
	class Text;
	class RectangleShape;
}

enum OptionsStateButtons
{
	GAME_OPTIONS_BUTTON,
	CONTROLS_OPTIONS_BUTTON,
	GRAPHICS_OPTIONS_BUTTON,
	AUDIO_OPTIONS_BUTTON
};

class OptionsState : public State {
public:
	OptionsState();
	bool Init();
	void Exit();

	bool Update();
	void Draw();

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);
	
	void changeOptionsState(int p_buttonID, int p_buttonState);

private:
	std::string next_state;
	ButtonList* m_buttonList;
	sf::RectangleShape* m_background;
	int m_currentResolution;
};

