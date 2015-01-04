#pragma once

#include <vector>
#include <map>
#include <functional>

#include "SFML\System\String.hpp"
#include "SFML\Window\Keyboard.hpp"
#include "SFML\Window\Mouse.hpp"
#include "SFML\Window\Event.hpp"

struct InputKeyEvent
{

};

struct InputMouseEvent
{
	int mouse_x;
	int mouse_y;
};

class Input
{
public:
	Input();
	~Input();
	static void setup();
	static void update();

	static bool pressed_once(std::string value, bool logical_and = false);
	static bool pressed(std::string value, bool logical_and = false);

	static void HandleKeyDown(sf::Keyboard::Key key);
	static void HandleKeyUp(sf::Keyboard::Key key);

	static void HandleMouseDown(sf::Mouse::Button button);
	static void HandleMouseUp(sf::Mouse::Button button);
	static void HandleWheel(sf::Event::MouseWheelEvent event);

	static void onKeyDown(std::string p_key, std::function<void(InputKeyEvent)> p_callback);
	static void onKeyUp(std::string p_key, std::function<void(InputKeyEvent)> p_callback);
	static void onMouseDown(std::string p_key, std::function<void(InputMouseEvent)> p_callback);
	static void onMouseUp(std::string p_key, std::function<void(InputMouseEvent)> p_callback);

	static void clearCallbacks();

	static void setMousePosition(sf::Vector2i p_mousePos);
private:
	static std::map<std::string, std::function<void(InputKeyEvent)>> m_onKeyDownCallbacks;
	static std::map<std::string, std::function<void(InputKeyEvent)>> m_onKeyUpCallbacks;
	static std::map<std::string, std::function<void(InputMouseEvent)>> m_onMouseDownCallback;
	static std::map<std::string, std::function<void(InputMouseEvent)>> m_onMouseUpCallback;
	static std::map<std::string, bool> pressed_keys;
	static std::map<std::string, bool> prev_pressed_keys;
	static std::vector<std::string> keycode_to_string;
	static std::vector<std::string> mousebuttoncode_to_string;
	static std::map<std::string, sf::Mouse::Button> buttoncode_to_string;
	static int mousex;
	static int mousey;
};

