#pragma once

#include <string>
#include "yaml-cpp\yaml.h"

class PlayerObject;
class AnimationManager;

class Config {
public:
	Config();
	static void parseFile(std::string value);
	static void saveFile(std::string value = "");
	static void configure(AnimationManager *animationManager);
	static std::string file;
	static YAML::Node root_node;
	static bool debug;
};