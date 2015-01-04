#include "Input.h"
#include "String.h"
#include "SFML\Window\Event.hpp"
#include <iostream>

std::map<std::string, std::function<void(InputKeyEvent)>> Input::m_onKeyDownCallbacks;
std::map<std::string, std::function<void(InputKeyEvent)>> Input::m_onKeyUpCallbacks;
std::map<std::string, std::function<void(InputMouseEvent)>> Input::m_onMouseDownCallback;
std::map<std::string, std::function<void(InputMouseEvent)>> Input::m_onMouseUpCallback;
std::map<std::string, bool> Input::pressed_keys;
std::map<std::string, bool> Input::prev_pressed_keys;
std::vector<std::string> Input::keycode_to_string;
std::vector<std::string> Input::mousebuttoncode_to_string;
std::map<std::string, sf::Mouse::Button> Input::buttoncode_to_string;
int Input::mousex;
int Input::mousey;

Input::Input()
{
}


Input::~Input()
{
}

void Input::setup()
{
	pressed_keys.clear();
	prev_pressed_keys.clear();
	keycode_to_string.clear();
	mousebuttoncode_to_string.clear();
	buttoncode_to_string.clear();
	keycode_to_string.resize(sf::Keyboard::KeyCount);
	mousebuttoncode_to_string.resize(sf::Mouse::ButtonCount);

	keycode_to_string[36] = "escape";
	keycode_to_string[37] = "left_control left_ctrl";
	keycode_to_string[38] = "left_shift";
	keycode_to_string[39] = "left_alt";
	keycode_to_string[40] = "left_system";
	keycode_to_string[41] = "right_control";
	keycode_to_string[42] = "right_shift";
	keycode_to_string[43] = "right_alt";
	keycode_to_string[44] = "right_system";
	keycode_to_string[45] = "menu";
	keycode_to_string[46] = "left_bracket";
	keycode_to_string[47] = "right_bracket";
	keycode_to_string[48] = "semicolon";
	keycode_to_string[49] = "comma";
	keycode_to_string[50] = "period";
	keycode_to_string[51] = "quote";
	keycode_to_string[52] = "slash";
	keycode_to_string[53] = "backslash";
	keycode_to_string[54] = "tilde";
	keycode_to_string[55] = "equal";
	keycode_to_string[56] = "dash";
	keycode_to_string[57] = "space";
	keycode_to_string[58] = "return enter";
	keycode_to_string[59] = "backspace";
	keycode_to_string[60] = "tab";
	keycode_to_string[61] = "page_up";
	keycode_to_string[62] = "page_down";
	keycode_to_string[63] = "end";
	keycode_to_string[64] = "home";
	keycode_to_string[65] = "insert";
	keycode_to_string[66] = "delete";
	keycode_to_string[67] = "add";
	keycode_to_string[68] = "substract";
	keycode_to_string[69] = "multiply";
	keycode_to_string[70] = "divide";
	keycode_to_string[71] = "left";
	keycode_to_string[72] = "right";
	keycode_to_string[73] = "up";
	keycode_to_string[74] = "down";
	
	keycode_to_string[100] = "pause break";

	std::vector<std::string> letters;
	letters.push_back("a");
	letters.push_back("b");
	letters.push_back("c");
	letters.push_back("d");
	letters.push_back("e");
	letters.push_back("f");
	letters.push_back("g");
	letters.push_back("h");
	letters.push_back("i");
	letters.push_back("j");
	letters.push_back("k");
	letters.push_back("l");
	letters.push_back("m");
	letters.push_back("n");
	letters.push_back("o");
	letters.push_back("p");
	letters.push_back("q");
	letters.push_back("r");
	letters.push_back("s");
	letters.push_back("t");
	letters.push_back("u");
	letters.push_back("v");
	letters.push_back("w");
	letters.push_back("x");
	letters.push_back("y");
	letters.push_back("z");
	std::vector<std::string> numbers;
	numbers.push_back("0");
	numbers.push_back("1");
	numbers.push_back("2");
	numbers.push_back("3");
	numbers.push_back("4");
	numbers.push_back("5");
	numbers.push_back("6");
	numbers.push_back("7");
	numbers.push_back("8");
	numbers.push_back("9");
	std::vector<std::string> fkeys;
	fkeys.push_back("f1");
	fkeys.push_back("f2");
	fkeys.push_back("f3");
	fkeys.push_back("f4");
	fkeys.push_back("f5");
	fkeys.push_back("f6");
	fkeys.push_back("f7");
	fkeys.push_back("f8");
	fkeys.push_back("f9");
	fkeys.push_back("f10");
	fkeys.push_back("f11");
	fkeys.push_back("f12");
	fkeys.push_back("f13");
	fkeys.push_back("f14");
	fkeys.push_back("f15");
	std::vector<std::string> numpadkeys;
	numpadkeys.push_back("numpad0");
	numpadkeys.push_back("numpad1");
	numpadkeys.push_back("numpad2");
	numpadkeys.push_back("numpad3");
	numpadkeys.push_back("numpad4");
	numpadkeys.push_back("numpad5");
	numpadkeys.push_back("numpad6");
	numpadkeys.push_back("numpad7");
	numpadkeys.push_back("numpad8");
	numpadkeys.push_back("numpad9");
	for (unsigned int i = 0; i < letters.size(); i++)    { keycode_to_string[i] = letters[i]; }
	for (unsigned int i = 0; i < numbers.size(); i++)    { keycode_to_string[26 + i] = numbers[i]; }
	for (unsigned int i = 0; i < numpadkeys.size(); i++) { keycode_to_string[75 + i] = numpadkeys[i]; }
	for (unsigned int i = 0; i < fkeys.size(); i++)      { keycode_to_string[85 + i] = fkeys[i]; }

	mousebuttoncode_to_string[0] = "left_mouse_button left_mouse";
	mousebuttoncode_to_string[1] = "right_mouse_button right_mouse";
	mousebuttoncode_to_string[2] = "middle_mouse_button middle_mouse";
}

void Input::update()
{
	pressed_keys["mouse_scroll_up"] = false;
	pressed_keys["mouse_scroll_down"] = false;
}

bool Input::pressed_once(std::string value, bool logical_and)
{
	if (pressed(value, logical_and))
	{
		if (!prev_pressed_keys[value])
		{
			prev_pressed_keys[value] = true;
			return true;
		}
		return false;
	}
	else {
		prev_pressed_keys[value] = false;
		return false;
	}
}
bool Input::pressed(std::string value, bool logical_and)
{
	std::vector<std::string> keys = String::explode(value, " ");
	bool pressed = false;
	
	if (!keys.empty()) {
		if (logical_and)
		{
			bool pressed_all = true;
			for (auto key : keys)
			{
				if (!pressed_keys[key] && pressed_all)
				{
					pressed_all = false;
				}
			}
			return pressed_all;
		}
		else {
			for (auto key : keys)
			{
				if (pressed_keys[key])
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Input::HandleKeyDown(sf::Keyboard::Key key)
{
	if (key == -1) return;
	std::vector<std::string> human_names = String::explode(keycode_to_string[key], " ");
	for (auto name : human_names)
	{
		pressed_keys[name] = true;
		auto it = m_onKeyDownCallbacks.find(name);
		if (it != m_onKeyDownCallbacks.end())
		{
			InputKeyEvent keyEvent;
			it->second(keyEvent);
		}
	}
}
void Input::HandleKeyUp(sf::Keyboard::Key key)
{
	if (key == -1) return;
	std::vector<std::string> human_names = String::explode(keycode_to_string[key], " ");
	for (auto name : human_names)
	{
		pressed_keys[name] = false;
		auto it = m_onKeyUpCallbacks.find(name);
		if (it != m_onKeyUpCallbacks.end())
		{
			InputKeyEvent keyEvent;
			it->second(keyEvent);
		}
	}
}

void Input::HandleMouseDown(sf::Mouse::Button button)
{
	std::vector<std::string> human_names = String::explode(mousebuttoncode_to_string[button], " ");
	for (auto name : human_names)
	{
		pressed_keys[name] = true;
		auto it = m_onMouseDownCallback.find(name);
		if (it != m_onMouseDownCallback.end())
		{
			InputMouseEvent mouseEvent;
			mouseEvent.mouse_x = mousex;
			mouseEvent.mouse_y = mousey;
			it->second(mouseEvent);
		}
	}
}
void Input::HandleMouseUp(sf::Mouse::Button button)
{
	std::vector<std::string> human_names = String::explode(mousebuttoncode_to_string[button], " ");
	for (auto name : human_names)
	{
		pressed_keys[name] = false; 
		auto it = m_onMouseUpCallback.find(name);
		if (it != m_onMouseUpCallback.end())
		{
			InputMouseEvent mouseEvent;
			mouseEvent.mouse_x = mousex;
			mouseEvent.mouse_y = mousey;
			it->second(mouseEvent);
		}
	}
}
void Input::HandleWheel(sf::Event::MouseWheelEvent event)
{
	if (event.delta > 0)
	{
		pressed_keys["mouse_scroll_up"] = true;
	}
	else
	{
		pressed_keys["mouse_scroll_up"] = false;
	}
	
	if (event.delta < 0)
	{
		pressed_keys["mouse_scroll_down"] = true;
	}
	else
	{
		pressed_keys["mouse_scroll_down"] = false;
	}
}
void Input::setMousePosition(sf::Vector2i p_mousePos)
{
	mousex = p_mousePos.x;
	mousey = p_mousePos.y;
}

void Input::onKeyDown(std::string p_key, std::function<void(InputKeyEvent)> p_callback)
{
	m_onKeyDownCallbacks.insert(std::make_pair(p_key, p_callback));
}
void Input::onKeyUp(std::string p_key, std::function<void(InputKeyEvent)> p_callback)
{
	m_onKeyUpCallbacks.insert(std::make_pair(p_key, p_callback));
}
void Input::onMouseDown(std::string p_key, std::function<void(InputMouseEvent)> p_callback)
{
	m_onMouseDownCallback.insert(std::make_pair(p_key, p_callback));
}
void Input::onMouseUp(std::string p_key, std::function<void(InputMouseEvent)> p_callback)
{
	m_onMouseUpCallback.insert(std::make_pair(p_key, p_callback));
}

void Input::clearCallbacks()
{
	m_onKeyDownCallbacks.clear();
	m_onKeyUpCallbacks.clear();
	m_onMouseDownCallback.clear();
	m_onMouseUpCallback.clear();
}
