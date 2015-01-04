#pragma once

#include "GuardState.h"

class GuardStateIdle: public GuardState
{
public:
	GuardStateIdle();
	bool Init();
	void Exit();

	bool Update(sf::Time deltatime);
	
	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

	void newPointSetup();
	void canRotateAgain();
	void newPointSetupWayBack();
	void canRotateAgainWayBack();
private:
	std::string next_state;
	bool initNewPoint;
	bool rotateDirection;
	bool rotate;
};

