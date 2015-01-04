#pragma once

#include "State.h"

class WinGameState : public State {
public:
	WinGameState();
	bool Init();
	void Exit();

	bool Update();
	void Draw();

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

private:
	std::string next_state;
};