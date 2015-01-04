#pragma once

#include "State.h"

class GameOverState : public State {
public:
	GameOverState();
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