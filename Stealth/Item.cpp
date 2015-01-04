#include "Item.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SpriteManager.h"

Item::Item()
{
	m_primaryIcon = new sf::Sprite();
	m_secondaryIcon = new sf::Sprite();
	m_spriteGround = new sf::Sprite();
	m_spriteWear = new sf::Sprite();
	m_wear = false;
	m_canBePickedUp = false;
	m_groundSpriteSize = sf::Vector2f(0, 0);
}

Item::~Item()
{
	delete m_primaryIcon;
	delete m_secondaryIcon;
	delete m_spriteGround;
	delete m_spriteWear;

	m_primaryIcon = nullptr;
	m_secondaryIcon = nullptr;
	m_spriteWear = nullptr;
	m_spriteGround = nullptr;
}


void Item::setType(int p_type)
{
	type = p_type;
}
int Item::getType()
{
	return type;
}
void Item::setPrimaryIconSprite(std::string path)
{
	m_primaryIcon->setTexture(*SpriteManager::loadImage(path));
}
void Item::setSecondaryIconSprite(std::string path)
{
	m_secondaryIcon->setTexture(*SpriteManager::loadImage(path));
}

sf::Sprite* Item::getPrimarySprite()
{
	return m_primaryIcon;
}
sf::Sprite* Item::getSecondarySprite()
{
	return m_secondaryIcon;
}
sf::Sprite* Item::getGroundSprite()
{
	return m_spriteGround;
}
sf::Sprite* Item::getWearSprite()
{
	return m_spriteWear;
}
bool Item::isWearing()
{
	return m_wear;
}
bool Item::canBePickedUp()
{
	return m_canBePickedUp;
}
void Item::setWear(bool p_value)
{
	m_wear = p_value;
}
void Item::setCanBePickedUp(bool p_value)
{
	m_canBePickedUp = p_value;
	sf::IntRect rect;
	rect.width = m_groundSpriteSize.x;
	rect.height = m_groundSpriteSize.y;
	if (m_canBePickedUp)
		rect.left = m_groundSpriteSize.x;
	else
		rect.left = 0;
	rect.top = 0;
	m_spriteGround->setTextureRect(rect);
}
sf::Vector2f Item::getGroundSpriteSize()
{
	return m_groundSpriteSize;
}
void Item::setGroundSpriteSize(sf::Vector2f p_size)
{
	m_groundSpriteSize = p_size;
}