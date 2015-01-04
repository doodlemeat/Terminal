#pragma once

#include <map>

namespace sf
{
	class Texture;
};

class SpriteManager
{
public:
	SpriteManager();
	~SpriteManager();

	static void init();
	static void cleanup();
	static sf::Texture* loadImage(std::string value);

private:
	static std::string m_path;
	static std::map<std::string, sf::Texture*> textures;
};

