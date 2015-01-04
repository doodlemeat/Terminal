#pragma once

#include "GuardState.h"
#include "SFML/System/Clock.hpp"

class GuardStateChase : public GuardState
{
public:
	GuardStateChase();
	bool Init();
	void Exit();

	bool Update(sf::Time deltatime);

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

	void newPointSetup();
	void canRotateAgain();

private:
	std::string next_state;
	sf::Clock timer;
	
	int nextPoint;

	bool initNewPoint;
	bool find_new_path;
	bool rotateDirection;
	bool rotate;
};