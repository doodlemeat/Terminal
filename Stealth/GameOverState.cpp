#include "State.h"
#include "SFML\System\String.hpp"
#include "SFML\Graphics\Font.hpp"
#include "Text.h"
#include "Window.h"
#include "GameOverState.h"
#include "Input.h"
#include <iostream>

GameOverState::GameOverState()
{

}

bool GameOverState::Init()
{
	std::cout << "Entered game over state" << std::endl;
	return true;
}
void GameOverState::Exit()
{
	Text::cleanup();
}

bool GameOverState::Update()
{
	if (Input::pressed_once("escape"))
	{
		setNextState("GameState");
		return false;
	}
	return true;
}

void GameOverState::Draw()
{
}

std::string GameOverState::Next()
{
	return next_state;
}
void GameOverState::setNextState(std::string state)
{
	next_state = state;
}
bool GameOverState::IsType(const std::string &type)
{
	return (type == "GameOverState");
}