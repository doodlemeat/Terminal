#include "App.h"
#include "Window.h"
#include "Input.h"
#include "Font.h"
#include "Text.h"
#include "SpriteManager.h"
#include "Audiomanager.h"
#include "State.h"
#include "SFML\System\String.hpp"
#include "SFML\Window\Event.hpp"
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Window\Mouse.hpp"
#include <iostream>

App::App()
{
	mRunning = true;
	states.clear();
}
App::~App()
{
}
void App::Init(std::string title, int width, int height)
{
	Font::configure();
	SpriteManager::init();
	Window::create("main", title, width, height);
	Input::setup();
	Audiomanager::setMFXPath("../music/");
	Audiomanager::setSFXPath("../sound/");
	Audiomanager::add(AUDIO_MFX, "alarm", "alarm.wav");
	Audiomanager::add(AUDIO_MFX, "mount_doom", "mount_doom.wav");

	Audiomanager::add(AUDIO_SFX, "step1", "step1.ogg");
	Audiomanager::add(AUDIO_SFX, "step2", "step2.ogg");
	Audiomanager::add(AUDIO_SFX, "step3", "step3.ogg");
	Audiomanager::add(AUDIO_SFX, "pang", "gun.ogg");
	Audiomanager::add(AUDIO_SFX, "rifle", "rifle.ogg");
	Audiomanager::add(AUDIO_SFX, "breath", "breath.ogg");
	Audiomanager::add(AUDIO_SFX, "success", "success.ogg");
	Audiomanager::add(AUDIO_SFX, "ammo_empty", "ammo_empty.ogg");
	Audiomanager::add(AUDIO_SFX, "coin", "coin.ogg");
	Audiomanager::add(AUDIO_SFX, "Button_Hover", "button_hover.ogg");
	Audiomanager::add(AUDIO_SFX, "Button_Down", "button_down.ogg");


	Audiomanager::createPlaylist("Game_Playlist");
	Audiomanager::getPlaylist("Game_Playlist")->addTrack("mrm.wav");
	Audiomanager::getPlaylist("Game_Playlist")->addTrack("midnight.wav");
}
void App::Cleanup()
{
	auto it = states.begin();
	while (it != states.end()) {
		delete (*it);
		++it;
	};
	Window::cleanup();
	Font::cleanup();
	Text::cleanup();
	SpriteManager::cleanup();
	Audiomanager::cleanup();
}
void App::Run()
{
	sf::Clock clock;
	while (mRunning)
	{
		sf::Time deltatime = clock.restart();
		Window::get("main")->deltatime = deltatime;
		HandleEvents();
		Logic();
		Render();
	}
}
void App::HandleEvents()
{
	sf::Event event;
	Input::setMousePosition(static_cast<sf::Vector2i>(Window::get("main")->getMousePosition()));
	Input::update();
	while (Window::get("main")->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			mRunning = false;
			break;
		case sf::Event::KeyPressed:
			Input::HandleKeyDown(event.key.code);
			break;
		case sf::Event::KeyReleased:
			Input::HandleKeyUp(event.key.code);
			break;
		case sf::Event::MouseButtonPressed:
			Input::HandleMouseDown(event.mouseButton.button);
			break;
		case sf::Event::MouseButtonReleased:
			Input::HandleMouseUp(event.mouseButton.button);
			break;
			case sf::Event::MouseWheelMoved:
			Input::HandleWheel(event.mouseWheel);
			break;

		default:
			break;
		}
	}
}
void App::Logic()
{
	if (current == nullptr) { return; };
	if (!current->Update()) {
		ChangeState();
	};
}
void App::Render()
{
	if (current == nullptr) { return; };
	Window::get("main")->clear(sf::Color(22, 4, 31));
	current->Draw();
	Window::get("main")->display();
}
void App::AddState(State* state)
{
	states.push_back(state);
}
void App::ChangeState()
{
	std::string next = current->Next();
	if (current != nullptr) {
		current->Exit();
		current = nullptr;
	};
	if (next.empty()) { 
		this->Cleanup();
		mRunning = false;
		return; 
	}

	for (unsigned int i = 0; i < states.size(); i++) {
		if (states[i]->IsType(next)) {
			current = states[i];
			current->Init();
			return;
		};
	};
}
void App::SetState(std::string type) {
	for (unsigned int i = 0; i < states.size(); i++) {
		if (states[i]->IsType(type)) {
			current = states[i];
			current->Init();
		};
	};
}