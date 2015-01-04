#include "Animation.h"
#include "SFML\Graphics\Texture.hpp"
#include "Config.h"
#include "SFML\System\Time.hpp"
#include "SFML\Graphics\Vertex.hpp"

#include <iostream>

int Animation::getSize()
{
	return m_frames.size();
}
sf::IntRect Animation::getFrame(int frame)
{
	return m_frames[frame];
}

/////////////////////////

AnimatedSprite::AnimatedSprite(bool paused)
{
	this->m_frame_duration = 0;
	this->m_animations.clear();
	this->currentFrame = 0;
	this->isPaused = paused;
	this->m_texture = nullptr;
	this->m_loopable = true;
}
void AnimatedSprite::update(sf::Time deltatime)
{
	if (!isPaused)
	{
		m_currentTime += deltatime.asMilliseconds();
		if (m_currentTime >= m_frame_duration)
		{
			m_currentTime = m_currentTime % m_frame_duration;

			if (currentFrame + 1 < current_animation->getSize())
				currentFrame++;
			else
			{
				if (m_loopable)
					currentFrame = 0;
				else
					isPaused = true;
			}
			setFrame(currentFrame, false);
		}
	}
}
void AnimatedSprite::addAnimation(Animation *animation)
{
	m_animations.insert(std::pair<std::string, Animation*>(animation->identifier, animation));
	setCurrentAnimation(animation->identifier);
}
void AnimatedSprite::setCurrentAnimation(std::string identifier)
{
	auto it = m_animations.find(identifier);
	if (it != m_animations.end() && current_animation != it->second) {
		current_animation = it->second;
		currentFrame = 0;
		m_texture = it->second->m_texture;
		m_frame_duration = it->second->frame_duration;
		setFrame(currentFrame);
	}
}
void AnimatedSprite::pause()
{
	isPaused = true;
}
void AnimatedSprite::stop()
{
	isPaused = true;
	currentFrame = 0;
	setFrame(currentFrame);
}
Animation* AnimatedSprite::getCurrentAnimation() 
{
	return current_animation;
}
sf::FloatRect AnimatedSprite::getLocalBounds()
{
	sf::IntRect rect = current_animation->getFrame(currentFrame);

	float width = static_cast<float>(std::abs(rect.width));
	float height = static_cast<float>(std::abs(rect.height));

	return sf::FloatRect(0.f, 0.f, width, height);

}
sf::FloatRect AnimatedSprite::getGlobalBounds()
{
	return getTransform().transformRect(getLocalBounds());
}
bool AnimatedSprite::isPlaying()
{
	return !isPaused;
}
int AnimatedSprite::getFrameDuration()
{
	return m_frame_duration;
}
void AnimatedSprite::setFrame(int newFrame, bool resetTime)
{
	sf::IntRect rect = current_animation->getFrame(currentFrame);

	m_vertices[0].position = sf::Vector2f(0.f, 0.f);
	m_vertices[1].position = sf::Vector2f(0.f, static_cast<float>(rect.height));
	m_vertices[2].position = sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
	m_vertices[3].position = sf::Vector2f(static_cast<float>(rect.width), 0.f);

	float left = static_cast<float>(rect.left) + 0.0001f;
	float right = left + static_cast<float>(rect.width);
	float top = static_cast<float>(rect.top);
	float bottom = top + static_cast<float>(rect.height);

	m_vertices[0].texCoords = sf::Vector2f(left, top);
	m_vertices[1].texCoords = sf::Vector2f(left, bottom);
	m_vertices[2].texCoords = sf::Vector2f(right, bottom);
	m_vertices[3].texCoords = sf::Vector2f(right, top);

	if (resetTime)
		m_currentTime = 0;
}
void AnimatedSprite::setLoopable(bool p_value)
{
	m_loopable = p_value;
}
void AnimatedSprite::draw(sf::RenderTarget &rt, sf::RenderStates states) const
{
	if (m_texture)
	{
		states.transform *= getTransform();
		states.texture = m_texture;
		rt.draw(m_vertices, 4, sf::Quads, states);
	}
}

////////////////////////////////

AnimationManager::AnimationManager()
{
	m_animations.clear();
	Config::configure(this);
}
AnimationManager::~AnimationManager()
{
	auto it = m_animations.begin();
	while (it != m_animations.end())
	{
		delete it->second;
		++it;
	}
	m_animations.clear();
}
bool AnimationManager::cleanup()
{
	return true;
}
bool AnimationManager::addAnimation(Animation* animation)
{
	auto it = m_animations.find(animation->identifier);
	if (it != m_animations.end())
	{
		std::cout << "Failed to add animation " << animation->identifier << ". Animation already exist." << std::endl;
		return false;
	}
	else
	{
		m_animations.insert(std::pair<std::string, Animation*>(animation->identifier, animation));
		return true;
	}
}
bool AnimationManager::deleteAnimation(std::string name)
{
	return true;
}
Animation* AnimationManager::get(std::string name)
{
	auto it = m_animations.find(name);
	if (it != m_animations.end())
	{
		it->second->identifier = name;
		return it->second;
	}
	return nullptr;
}