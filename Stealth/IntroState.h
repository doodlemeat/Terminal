#pragma once

#include "State.h"
#include "SFML\Graphics\Text.hpp"
#include "SFML\System\Clock.hpp"

class SpriteManager;
namespace sf
{
	class Sprite;
}

struct IntroImage
{
	sf::Sprite* sprite;
	sf::Clock alive;
	int time;
	int scale_time;
	bool isDead;
	bool isReallyDead;
	sf::Vector2f velocity;
	void update(sf::Time deltatime);
};

class IntroState : public State {
public:
	IntroState();
	bool Init();
	void Exit();

	bool Update();
	void Draw();

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);
private:
	std::string next_state;

	SpriteManager* m_spriteManager;
	int m_songLength;
	int m_currentImage;
	std::vector<IntroImage*> m_introImages;
};

