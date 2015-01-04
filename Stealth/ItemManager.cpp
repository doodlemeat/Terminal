#include "ItemManager.h"
#include "WeaponManager.h"
#include "Config.h"
#include "Item.h"
#include <iostream>
#include "Weapon.h"
#include "SFML\Graphics\Sprite.hpp"
#include "SFML\Graphics\RenderTarget.hpp"


ItemManager::ItemManager()
{
	m_weaponManager = new WeaponManager();
	m_weaponManager->configure();
	configure();
}


ItemManager::~ItemManager()
{
	auto it = m_items.begin();
	while (it != m_items.end())
	{
		delete *it;
		++it;
	}
	m_items.clear();

	delete m_weaponManager;
	m_weaponManager = nullptr;
}

void ItemManager::configure()
{
	if (Config::root_node["general"])
	{
		if (Config::root_node["general"]["item_path"])
		{
			m_itemSpritePath = Config::root_node["general"]["item_path"].as<std::string>();
		}
		if (Config::root_node["general"]["weapon_path"])
		{
			m_weaponManager->setSpritePath(m_itemSpritePath + Config::root_node["general"]["weapon_path"].as<std::string>());
		}
		m_coinThrowSpeed = Config::root_node["general"]["coin_throw_speed"].as<float>();
		m_coin_ripple_radius = Config::root_node["general"]["coin_ripple_radius"].as<float>();
	}
}

void ItemManager::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &item: m_items)
	{
		if (!item->isWearing())
		{
			target.draw(*item->getGroundSprite());
		}
	}
}

void ItemManager::addItem(Item* p_item)
{
	m_items.push_back(p_item);
	if (p_item->getType() == ITEM_WEAPON)
	{
		m_weaponManager->add(static_cast<Weapon*>(p_item));
	}
}
WeaponManager* ItemManager::getWeaponManager()
{
	return m_weaponManager;
}
std::string ItemManager::getSpritePath()
{
	return m_itemSpritePath;
}
std::vector<Item*> ItemManager::getItems()
{
	return m_items;
}

int ItemManager::getCoinSpeed()
{
	return m_coinThrowSpeed;
}

float ItemManager::getSoundRippleRadius()
{
	return m_coin_ripple_radius;
}

void ItemManager::deleteItem(Item* p_pointer)
{
	for (int i = 0; i < m_items.size(); i++)
	{
		if (p_pointer == m_items[i])
		{
			delete m_items[i];
			m_items[i] = nullptr;
			m_items.erase(m_items.begin() + i);
			break;
		}
	}
}

std::vector<Item*> ItemManager::getItemsByType(int p_itemType)
{
	std::vector<Item*> items;
	for (auto &item : m_items)
	{
		if (item->getType() == p_itemType)
		{
			items.push_back(item);
		}
	}
	return items;
}
