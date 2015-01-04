#pragma once

#include <string>
#include "SFML/System/Vector2.hpp"

enum ItemType
{
	ITEM_WEAPON,
	ITEM_KEYCARD,
	ITEM_COIN,
	ITEM_DISG
};

namespace sf
{
	class Sprite;
}

class Item
{
public:
	Item();
	virtual ~Item();

	void setType(int p_type);
	int getType();

	void setPrimaryIconSprite(std::string path);
	void setSecondaryIconSprite(std::string path);
	
	sf::Sprite* getPrimarySprite();
	sf::Sprite* getSecondarySprite();
	sf::Sprite* getGroundSprite();
	sf::Sprite* getWearSprite();

	bool isWearing();
	bool canBePickedUp();
	sf::Vector2f getGroundSpriteSize();
	void setWear(bool p_value);
	void setCanBePickedUp(bool p_value);
	void setGroundSpriteSize(sf::Vector2f p_size);
private:
	int type;
	sf::Sprite* m_spriteGround;
	sf::Sprite* m_spriteWear;
	sf::Sprite* m_primaryIcon;
	sf::Sprite* m_secondaryIcon;
	bool m_wear;
	bool m_canBePickedUp;
	sf::Vector2f m_groundSpriteSize;
};

