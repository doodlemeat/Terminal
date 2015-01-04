#pragma once

#include "SFML\Graphics.hpp"
#include "Animation.h"
#include <functional>
#include <map>
#include <vector>

class Button;

class ButtonData
{
public:
	ButtonData();
	virtual ~ButtonData();
};

enum ButtonState
{
	NORMAL,
	HOVER,
	DOWN,
	ACTIVE
};

struct ButtonCallbackList
{
	void addCallback(ButtonState p_state, std::function<void(int)> callback);
	std::map<ButtonState, std::function<void(int)>> m_callbacks;
};

class ButtonList: public sf::Drawable
{
public:
	ButtonList();
	~ButtonList();

	void update();
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void addButton(Button* p_button);
	void handleCallbacks(int p_buttonID, int p_buttonState);
	void setCallbackHandler(std::function<void(int, int)> p_callbackHandler);
	void setPosition(float p_x, float p_y);
	void setPosition(sf::Vector2f p_position);
	void updatePosition();
	void setToggleable(bool p_value); 
	void deActivateButtons();
	void toggleButton(int p_buttonIndex);

	sf::Vector2f getSize();
	void setScale(sf::Vector2f scale);
	void scaleSize(sf::Vector2f scale);
private:
	int m_currentActive;
	std::function<void(int, int)> m_callbackHandler;
	std::vector<Button*> m_buttons;
	bool m_toggleable;
	sf::Vector2f m_position;
};

class Button: public sf::Drawable
{
	friend class ButtonList;
public:

	Button();
	~Button();

	sf::Sprite* getSprite();

	void update();
	void update(sf::Vector2i m_position);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setSize(int p_w, int p_h);
	void addCallback(ButtonState p_state, std::function<void(int)> callback);
	void addCallBacks(ButtonCallbackList p_callbackList);
	void setToggleable(bool p_value);
	void setCallbackHandler(std::function<void(int, int)> callback);
	void setRawCallbackHandler(std::function<void(Button*, int)> callback);
	virtual void setID(int p_id);
	virtual void setClickableArea();
	void setClickableArea(sf::Vector2f p_position, sf::Vector2f p_extension);
	void setData(ButtonData* data);
	void scaleSize(sf::Vector2f scale);
	void setScale(sf::Vector2f p_scale);
	sf::Vector2f getCenter();
	int getWidth();
	int getHeight();

	template <class T> T getData()
	{
		return static_cast<T>(m_data);
	}

protected:
	sf::IntRect m_clickableArea;

private:
	ButtonData* m_data;
	std::function <void(int, int)> m_callbackHandler;
	std::function <void(Button*, int)> m_rawCallbackHandler;
	std::map<ButtonState, std::function<void(int)>> m_callbacks;

	void changeStyle(ButtonState state);
	void runCallbacks(ButtonState state);
	ButtonState m_currentState;
	sf::Sprite* m_sprite;

	sf::Vector2f m_scale;
	sf::Vector2i m_mousePosition;

	int m_width;
	int m_height;
	int m_id;

	bool m_customMousePosDef;
	bool pressed;
	bool clickStarted;
	bool m_toggleable;
	bool m_isActive;
	bool m_useCallbackHandler;
	bool m_useRaw;
};

class TextButton : public Button {
public:
	TextButton();
	~TextButton();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setClickableArea();
	sf::Text* getText();
private:
	sf::Text* m_text;
};