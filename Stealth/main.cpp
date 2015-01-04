#include "App.h"
#include "Config.h"

#include "IntroState.h"
#include "MenuState.h"
#include "OptionsState.h"
#include "GameState.h"
#include "GameOverState.h"
#include "WinGameState.h"
#include <iostream>

int main(int argc, char* argv[])
{
	std::vector<std::string> arguments;
	int arguments_count = argc;
	while (arguments_count > 0)
	{
		arguments.push_back(argv[arguments_count-1]);
		arguments_count--;
	}

	if (std::find(arguments.begin(), arguments.end(), "debug") != arguments.end())
		Config::debug = true;
	else
		Config::debug = false;

	// Init configuration file
	Config::parseFile("../configuration/config.yml");
	App app;
	app.Init("Terminal", Config::root_node["general"]["default_resolution_x"].as<int>(), Config::root_node["general"]["default_resolution_y"].as<int>());

	// Add states
	app.AddState(new IntroState);
	app.AddState(new MenuState);
	app.AddState(new OptionsState);
	app.AddState(new GameState);
	app.AddState(new GameOverState);
	app.AddState(new WinGameState);

	app.SetState(Config::root_node["general"]["start_state"].as<std::string>());

	app.Run();

	return ~15;
}