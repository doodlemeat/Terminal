#pragma once

#include "GuardState.h"
#include "SFML\System\Clock.hpp"

class GuardStateSuspicious : public GuardState
{
public:
	GuardStateSuspicious();
	bool Init();
	void Exit();

	bool Update(sf::Time deltatime);
	void newPointSetup();
	void canRotateAgain();

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

private:
	std::string next_state; 
	bool initNewPoint;
	bool find_new_path;
	bool rotateDirection;
	bool rotate;
	bool wait;
	bool give_up;
	int start_rotation;
	int rotated_degrees;
};