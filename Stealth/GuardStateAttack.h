#pragma once

#include "GuardState.h"

class GuardStateAttack : public GuardState
{
public:
	GuardStateAttack();
	bool Init();
	void Exit();

	bool Update(sf::Time deltatime);

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

private:
	std::string next_state;
};