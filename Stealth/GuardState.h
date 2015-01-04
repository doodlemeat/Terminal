#pragma once
#include "SFML\System\Time.hpp"
#include <string>

class Guard;

class GuardState
{
public:
	virtual ~GuardState() {};

	virtual bool Init() = 0;
	virtual void Exit() = 0;
	virtual bool Update(sf::Time deltatime) = 0;

	virtual std::string Next() = 0;
	virtual void setNextState(std::string state) = 0;
	virtual bool IsType(const std::string &type) = 0;

	Guard* m_guard;
};