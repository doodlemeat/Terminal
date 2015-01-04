#include "SpriteManager.h"

#include "SFML\System\String.hpp"
#include "SFML\Graphics\Texture.hpp"
#include "Config.h"
#include <iostream>

std::map<std::string, sf::Texture*> SpriteManager::textures;
std::string SpriteManager::m_path;

SpriteManager::SpriteManager()
{
	
}


SpriteManager::~SpriteManager()
{
}

void SpriteManager::init()
{
	m_path = Config::root_node["general"]["sprite_path"].as<std::string>();
}

void SpriteManager::cleanup()
{
	auto it = textures.begin();
	while (it != textures.end())
	{
		delete it->second;
		it = textures.erase(it);
	}
}

sf::Texture* SpriteManager::loadImage(std::string value)
{
	auto it = textures.find(value);
	if (it == textures.end())
	{
		sf::Texture* texture = new sf::Texture();
		textures.insert(std::pair<std::string, sf::Texture*>(value, texture));
		if (!texture->loadFromFile(std::string(m_path + value)))
		{
			std::cout << "Failed to load image " << m_path + value << std::endl;
			return nullptr;
		}
		else
		{
			return texture;
		}
	}
	return it->second;
}
