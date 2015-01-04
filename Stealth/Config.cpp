#include "Config.h"
#include "yaml-cpp\yaml.h"
#include "SpriteManager.h"
#include "Animation.h"

#include <iostream>
#include <fstream>

std::string Config::file;
YAML::Node Config::root_node;
bool Config::debug;

Config::Config()
{
}
void Config::parseFile(std::string value)
{
	/* Save the path to a member variable */
	file = value;
	
	/* Create an std::ifstream */
	std::ifstream ifile(value);

	/* Check if its valid(that the file exist) */
	if (ifile) {
		std::cout << "Successfully loaded configuration file " << value << std::endl;
		
		/* Use yaml-cpp to parse the file */
		root_node = YAML::LoadFile(value);
	}
	else
	{
		std::cout << "Failed to load configuration file " << value << std::endl;
	}

	bool fullscreen = root_node["general"]["fullscreen"].as<bool>();

}
void Config::configure(AnimationManager *animationManager)
{
	/* Make sure the animations category exists */
	if (root_node["animations"])
	{
		/* Iterate trough each category(each animation) */
		for (YAML::iterator it = root_node["animations"].begin(); it != root_node["animations"].end(); ++it)
		{
			/* Create an animation object */
			Animation* animation = new Animation();

			/* Grab the identifier/key which is used to identify this animation */
			animation->identifier = it->first.as<std::string>();
			
			/* Grab the image and load it on the fly as a texture */
			animation->m_texture = SpriteManager::loadImage(it->second["image"].as<std::string>());

			/* Fetch the frame_duration */
			animation->frame_duration = it->second["frame_duration"].as<int>();

			/* We can use the operator [] to select which element to choose e.g. [128, 128] has 2 elements */
			int frame_width = it->second["frame_size"][0].as<int>();
			int frame_height = it->second["frame_size"][1].as<int>();
			
			/* Clear the frames vector in case there is some garbage down there */
			animation->m_frames.clear();
			
			/* Iterate trough every frame */
			for (unsigned int i = 0; i < it->second["frames"].size(); i++)
			{
				/* Fetch the position of the frame in the image */
				int x = it->second["frames"][i][0].as<int>();
				int y = it->second["frames"][i][1].as<int>();

				/* Setup an sf::Intrect which is used to define the region of the frame in the image */
				sf::IntRect rect;
				rect.left = x;
				rect.width = frame_width;
				rect.top = y;
				rect.height = frame_height;

				/* Save each frame into the frame vector */
				animation->m_frames.push_back(rect);
			}

			/* Save each animation into the animation manager */
			animationManager->addAnimation(animation);
		}
	}
}
void Config::saveFile(std::string value)
{
	YAML::Emitter emitter;
	emitter.SetIndent(4);
	emitter << root_node;
	if (value.empty())
		value = file;
	std::ofstream fout(value);
	fout << emitter.c_str();
	fout.close();
}