#include "State.h"
#include "SFML\System\String.hpp"
#include "SFML\Graphics\Font.hpp"
#include "Text.h"
#include "Window.h"
#include "WinGameState.h"
#include "Input.h"
#include <iostream>

WinGameState::WinGameState()
{

}

bool WinGameState::Init()
{
	std::cout << "Entered win game state" << std::endl;
	return true;
}
void WinGameState::Exit()
{
	Text::cleanup();
}

bool WinGameState::Update()
{
	if (Input::pressed_once("escape"))
	{
		setNextState("GameState");
		return false;
	}
	return true;
}

void WinGameState::Draw()
{
}

std::string WinGameState::Next()
{
	return next_state;
}
void WinGameState::setNextState(std::string state)
{
	next_state = state;
}
bool WinGameState::IsType(const std::string &type)
{
	return (type == "WinGameState");
}