#pragma once

#include "SFML/Graphics/Drawable.hpp"
#include <vector>

class Item;
class WeaponManager;

class ItemManager: public sf::Drawable
{
public:
	ItemManager();
	~ItemManager();

	void configure();
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void addItem(Item* p_item);
	WeaponManager* getWeaponManager();
	std::string getSpritePath();
	std::vector<Item*> getItems();
	int getCoinSpeed();
	float getSoundRippleRadius();
	void deleteItem(Item* p_pointer);
	std::vector<Item*> getItemsByType(int p_itemType);
private:
	std::string m_itemSpritePath;
	std::vector<Item*> m_items;
	std::vector<Item*> m_itemsOnGround;
	WeaponManager* m_weaponManager;
	float m_coinThrowSpeed;
	float m_coin_ripple_radius;
};

