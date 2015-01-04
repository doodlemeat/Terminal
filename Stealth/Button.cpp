#include "Button.h"
#include "Input.h"
#include "Window.h"
#include "Audiomanager.h"

#include <iostream>

Button::Button()
{
	m_width = 100;
	m_height = 30;
	m_sprite = new sf::Sprite();
	m_sprite->setTextureRect(sf::IntRect(0, 0, m_width, m_height));
	m_currentState = NORMAL;
	pressed = false;
	m_toggleable = false;
	clickStarted = false;
	m_isActive = false;
	m_useCallbackHandler = false;
	m_useRaw = false;
	m_customMousePosDef = false;
	m_scale = sf::Vector2f(1, 1);
}
Button::~Button()
{
	delete m_sprite;
	m_sprite = nullptr;
}

void Button::update(sf::Vector2i m_position)
{
	m_mousePosition.x = m_position.x;
	m_mousePosition.y = m_position.y;
	m_customMousePosDef = true;
	update();
}
void Button::update()
{
	setClickableArea();
	sf::Vector2i mousePosition;
	if (m_customMousePosDef)
	{
		mousePosition = m_mousePosition;
	}
	else
	{
		mousePosition = static_cast<sf::Vector2i>(Window::get("main")->getMousePositionWindow());
	}
	bool mouseButton = Input::pressed("left_mouse_button");
	if (!pressed && mouseButton)
	{
		if (m_clickableArea.contains(mousePosition))
		{
			clickStarted = true;
			changeStyle(DOWN);
		}
		pressed = true;
	}
	else if (pressed && !mouseButton)
	{
		if (clickStarted && m_clickableArea.contains(mousePosition))
		{
			runCallbacks(DOWN);
			if (m_toggleable)
			{
				m_isActive = !m_isActive;
				if (m_isActive)
				{
					changeStyle(ACTIVE);
				}
			}
		}
		clickStarted = false;
		pressed = false;
	}
	else if (!clickStarted && !m_isActive)
	{
		if (m_clickableArea.contains(mousePosition))
		{
			changeStyle(HOVER);
		}
		else
		{
			changeStyle(NORMAL);
		}
	}
}
sf::Sprite* Button::getSprite()
{
	return m_sprite;
}
void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*m_sprite);
}
void Button::setSize(int p_w, int p_h)
{
	m_width = p_w;
	m_height = p_h;
	m_sprite->setTextureRect(sf::IntRect(0, 0, m_width, m_height));
}
void Button::addCallback(ButtonState p_state, std::function<void(int)> callback)
{
	m_callbacks.insert(std::make_pair(p_state, callback));
}
void Button::addCallBacks(ButtonCallbackList p_callbackList)
{
	m_callbacks.insert(p_callbackList.m_callbacks.begin(), p_callbackList.m_callbacks.end());
}
void Button::setToggleable(bool p_value)
{
	m_toggleable = p_value;
}
void Button::setCallbackHandler(std::function<void(int, int)> callback)
{
	m_useCallbackHandler = true;
	m_callbackHandler = callback;
}
void Button::setID(int p_id)
{
	m_id = p_id;
}
sf::Vector2f Button::getCenter()
{
	return sf::Vector2f(
		m_sprite->getPosition().x + m_width / 2 + m_sprite->getOrigin().x,
		m_sprite->getPosition().y + m_height / 2 + m_sprite->getOrigin().y);
}
int Button::getWidth()
{
	return m_width;
}
int Button::getHeight()
{
	return m_height;
}
void Button::changeStyle(ButtonState p_state)
{
	if (p_state != m_currentState)
	{
		switch (p_state)
		{
		case HOVER:
			Audiomanager::sfx("Button_Hover")->play();
			break;
		case DOWN:
			Audiomanager::sfx("Button_Down")->play();
		}
		m_currentState = p_state;
		sf::IntRect rect;
		rect.left = 0;
		rect.top = m_currentState * m_height;
		rect.width = m_width * m_scale.x;
		rect.height = m_height * m_scale.y;
		m_sprite->setTextureRect(rect);
	}
}
void Button::runCallbacks(ButtonState p_state)
{
	if (m_useRaw)
	{
		m_rawCallbackHandler(this, p_state);
		return;
	}
	if (m_useCallbackHandler)
	{
		m_callbackHandler(m_id, p_state);
		return;
	}

	auto it = m_callbacks.find(p_state);
	if (it != m_callbacks.end())
	{
		it->second(0);
	}
}
void Button::setClickableArea()
{
	m_clickableArea = sf::IntRect(m_sprite->getPosition().x, m_sprite->getPosition().y, m_width, m_height);
}
void Button::setClickableArea(sf::Vector2f p_position, sf::Vector2f p_extension)
{
	m_clickableArea = sf::IntRect(p_position.x, p_position.y, p_extension.x, p_extension.y);
}
void Button::setRawCallbackHandler(std::function<void(Button*, int)> callback)
{
	m_rawCallbackHandler = callback;
	m_useRaw = true;
}
void Button::setData(ButtonData* data)
{
	m_data = data;
}
void Button::scaleSize(sf::Vector2f scale)
{
	setClickableArea(m_sprite->getPosition(), sf::Vector2f((m_width * scale.x), (m_height * scale.y)));
}

void Button::setScale(sf::Vector2f p_scale)
{
	m_scale = p_scale;
}

ButtonList::ButtonList()
{
	m_currentActive = 0;
	m_toggleable = false;
}
ButtonList::~ButtonList()
{

}
void ButtonList::update()
{
	std::vector<bool> activeStates(m_buttons.size(), false);
	for (int i = 0; i < m_buttons.size(); i++) {
		if (m_buttons[i]->m_isActive)
		{
			activeStates[i] = true;
		}
		else
		{
			activeStates[i] = false;
		}
	}
	for (int i = 0; i < m_buttons.size(); i++) {
		m_buttons[i]->update();
		if (m_buttons[i]->m_isActive && !activeStates[i])
		{
			deActivateButtons();
			toggleButton(i);
		}
	}
}
void ButtonList::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (Button* button : m_buttons)
	{
		button->draw(target, states);
	}
}
void ButtonList::addButton(Button* p_button)
{
	m_buttons.push_back(p_button);
}
void ButtonList::handleCallbacks(int p_buttonID, int p_buttonState)
{
	std::cout << "hej" << std::endl;
	m_callbackHandler(p_buttonID, p_buttonState);
}
void ButtonList::setCallbackHandler(std::function<void(int, int)> p_callbackHandler)
{
	m_callbackHandler = p_callbackHandler;
	for(Button* button: m_buttons)
	{
		button->setCallbackHandler(std::bind(&ButtonList::handleCallbacks, this, std::placeholders::_1, std::placeholders::_2));
	}
}
void ButtonList::setPosition(float p_x, float p_y)
{
	m_position.x = p_x;
	m_position.y = p_y;
	updatePosition();
}
void ButtonList::setPosition(sf::Vector2f p_position)
{
	m_position = p_position;
	updatePosition();
}
void ButtonList::updatePosition()
{
	for (int i = 0; i < m_buttons.size(); i++)
	{
		m_buttons[i]->getSprite()->setPosition(
			m_position.x + i * m_buttons[i]->getSprite()->getGlobalBounds().width,
			m_position.y
		);
	}
}
void ButtonList::setToggleable(bool p_value)
{
	m_toggleable = p_value;
}
void ButtonList::deActivateButtons()
{
	for (int i = 0; i < m_buttons.size(); i++)
	{
		m_buttons[i]->m_isActive = false;
		m_buttons[i]->pressed = false;
		m_buttons[i]->clickStarted = false;
		m_buttons[i]->changeStyle(NORMAL);
	}
}
void ButtonList::toggleButton(int p_buttonIndex)
{
	m_buttons[p_buttonIndex]->m_isActive = true;
	m_buttons[p_buttonIndex]->changeStyle(ACTIVE);
}
sf::Vector2f ButtonList::getSize()
{
	return sf::Vector2f(m_buttons[0]->m_width, m_buttons[0]->m_height);
}

void ButtonList::setScale(sf::Vector2f scale)
{
	for (auto &button : m_buttons)
	{
		button->getSprite()->setScale(scale);
	}
}

void ButtonList::scaleSize(sf::Vector2f scale)
{
	for (auto &button : m_buttons)
	{
		button->scaleSize(scale);
	}
}

void ButtonCallbackList::addCallback(ButtonState p_state, std::function<void(int)> p_callback)
{
	m_callbacks.insert(std::make_pair(p_state, p_callback));
}

TextButton::TextButton()
{
	m_text = new sf::Text();
}

TextButton::~TextButton()
{
	delete m_text;
	m_text = nullptr;
}

void TextButton::setClickableArea()
{
	m_clickableArea = sf::IntRect(static_cast<int>(m_text->getPosition().x), static_cast<int>(m_text->getPosition().y), 100, 100);
}

sf::Text* TextButton::getText()
{
	return m_text;
}

void TextButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*m_text);
}

ButtonData::ButtonData()
{

}

ButtonData::~ButtonData()
{

}
