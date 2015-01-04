#include "State.h"
#include "Window.h"
#include "IntroState.h"
#include "Font.h"
#include "Config.h"
#include "Input.h"
#include "Button.h"
#include "SpriteManager.h"
#include "SFML\Graphics.hpp"
#include "Audiomanager.h"
#include <iostream>
#include "Thor/Math/Random.hpp"
#include "Math.h"

IntroState::IntroState()
{
	m_spriteManager = nullptr;
}

bool IntroState::Init()
{
	m_spriteManager = new SpriteManager();
	m_currentImage = 0;
	Audiomanager::mfx("mount_doom")->play();
	

	if (Config::root_node["general"])
	{
		int default_resolution_x = Config::root_node["general"]["default_resolution_x"].as<int>();
		int default_resolution_y = Config::root_node["general"]["default_resolution_y"].as<int>();

		std::cout << default_resolution_x << std::endl;
		std::cout << default_resolution_y << std::endl;

		m_songLength = Config::root_node["general"]["intro_untagged_part_of_new_mount_doom_length_in_ms"].as<int>();
		for (unsigned int i = 0; i < Config::root_node["general"]["intro_images"].size(); i++)
		{
			IntroImage* introImage = new IntroImage();
			introImage->sprite = new sf::Sprite();
			introImage->sprite->setTexture(*m_spriteManager->loadImage(Config::root_node["general"]["intro_images"][i].as<std::string>()));

			// scale down if we have to
			sf::Vector2f scale(1.f, 1.f);
			if (introImage->sprite->getGlobalBounds().width > default_resolution_x)
			{
				scale.x = default_resolution_x / introImage->sprite->getGlobalBounds().width;
			}
			if (introImage->sprite->getGlobalBounds().height > default_resolution_y)
			{
				scale.y = default_resolution_y / introImage->sprite->getLocalBounds().height;
			}
			introImage->sprite->setOrigin(introImage->sprite->getGlobalBounds().width / 2, introImage->sprite->getGlobalBounds().height / 2);
			introImage->sprite->setPosition(default_resolution_x / 2, default_resolution_y / 2);
			introImage->sprite->setScale(scale);
			int angle = thor::random(-4, 4);
			introImage->sprite->setRotation(angle);

			/* --------------------------------- */
			
			// Get random direction
			int vel_angle = thor::random(0, 360);

			// Find direction
			float dir_x = Math::cos(vel_angle);
			float dir_y = Math::sin(vel_angle);

			float length = Math::sqrt(dir_x * dir_x + dir_y * dir_y);

			float dir_x_normalized = dir_x / length;
			float dir_y_normalized = dir_x / length;

			float speed = 0.6;
			sf::Vector2f velocity(dir_x * speed, dir_y * speed);
			introImage->velocity = velocity;

			/* --------------------------------- */
			introImage->isDead = false;
			introImage->isReallyDead = false;

			introImage->time = 0;
			introImage->time -= (Math::log(20 / Math::max(introImage->sprite->getGlobalBounds().width, introImage->sprite->getGlobalBounds().height)) / Math::log(0.9f)) * 16.7f;

			m_introImages.push_back(introImage);
		}

		// Get image time5
		float imageTime = m_songLength / m_introImages.size();
		int length = 0;
		for (auto &image : m_introImages)
		{	
			image->time += imageTime;
			length += image->time;
		}
	}

	m_introImages[m_currentImage]->alive.restart();
	return true;
}
void IntroState::Exit()
{
	
}

bool IntroState::Update()
{
	if (Input::pressed("space escape "))
	{
		setNextState("MenuState");
		return false;
	}
	m_introImages[m_currentImage]->update(Window::get("main")->deltatime);
	if (m_introImages[m_currentImage]->isReallyDead)
	{
		m_currentImage++;
		if (m_currentImage == m_introImages.size())
		{
			setNextState("MenuState");
			return false;
		}
		m_introImages[m_currentImage]->alive.restart();
	}
	return true;
}

void IntroState::Draw()
{
	Window::get("main")->clear(sf::Color::Black);
	Window::get("main")->draw(*m_introImages[m_currentImage]->sprite);
}

std::string IntroState::Next()
{
	return next_state;
}
void IntroState::setNextState(std::string state)
{
	next_state = state;
}
bool IntroState::IsType(const std::string &type)
{
	return (type == "IntroState");
}

void IntroImage::update(sf::Time deltatime)
{
	if (alive.getElapsedTime().asMilliseconds() >= time && !isDead)
	{
		isDead = true;
	}

	if (isDead && !isReallyDead)
	{
		sprite->scale(0.9, 0.9);
		sprite->move(velocity);
		if (sprite->getGlobalBounds().width <= 20 && sprite->getGlobalBounds().height <= 20)
		{
			isReallyDead = true;
		}
	}
	else
	{
		sprite->move(velocity);
	}
}
