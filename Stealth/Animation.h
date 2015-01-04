#pragma once

#include <vector>
#include <map>
#include "SFML\Graphics\Rect.hpp"
#include "SFML\Graphics\RenderStates.hpp"
#include "SFML\Graphics\RenderTarget.hpp"
#include "SFML\Graphics\Drawable.hpp"
#include "SFML\Graphics\Transformable.hpp"
#include "SFML\System\Time.hpp"

class Config;

namespace sf
{
	class Texture;
};

struct Animation
{
	std::vector<sf::IntRect> m_frames;
	int frame_duration;
	sf::Texture* m_texture;
	std::string identifier;

	int getSize();
	sf::IntRect getFrame(int frame);
};

class AnimatedSprite: public sf::Drawable, public sf::Transformable
{
public:
	AnimatedSprite(bool paused = false);
	void update(sf::Time deltatime);
	void addAnimation(Animation *animation);
	void setCurrentAnimation(std::string identifier);
	void play();
	void pause();
	void stop();
	Animation* getCurrentAnimation();
	sf::FloatRect getLocalBounds();
	sf::FloatRect getGlobalBounds();
	bool isPlaying();
	int getFrameDuration();
	void setFrame(int newFrame, bool resetTime = true);
	void setLoopable(bool p_value);

private:
	std::map<std::string, Animation*> m_animations;
	Animation* current_animation;
	int m_frame_duration;
	int m_currentTime;
	int currentFrame;
	bool isPaused;
	bool m_loopable;
	const sf::Texture* m_texture;
	sf::Vertex m_vertices[4];

	virtual void draw(sf::RenderTarget &rt, sf::RenderStates states) const;
};

class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();

	bool cleanup();
	bool addAnimation(Animation* animation);
	bool deleteAnimation(std::string name);
	Animation* get(std::string name);

private:
	std::map<std::string, Animation*> m_animations;
};