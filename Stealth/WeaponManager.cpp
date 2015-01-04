#include "WeaponManager.h"
#include "Config.h"
#include <iostream>
#include "SpriteManager.h"
#include "SFML/Graphics/Sprite.hpp"
#include "Weapon.h"
#include "SFML/Graphics/RenderTarget.hpp"


WeaponSkeleton::WeaponSkeleton()
{
	name = "";
	fire_rate = 0;
	damage = 0;
	ammo = 0;
	bullet_spread = 0;
	weight = 0;
	auto_fire = false;
}

WeaponManager::WeaponManager()
{
}


WeaponManager::~WeaponManager()
{
}


void WeaponManager::configure()
{
	YAML::Node weapons_node = Config::root_node["items"]["weapons"];
	if (Config::root_node["items"]["weapons"])
	{
		if (weapons_node["guns"])
		{
			for (YAML::iterator it = weapons_node["guns"].begin(); it != weapons_node["guns"].end(); ++it)
			{
				WeaponSkeleton* wpnSkeleton = new WeaponSkeleton();
				wpnSkeleton->name = it->first.as<std::string>();
				if (it->second["ammo"])
					wpnSkeleton->ammo = it->second["ammo"].as<int>();
				if (it->second["fire_rate"])
					wpnSkeleton->fire_rate = it->second["fire_rate"].as<int>();
				if (it->second["damage"])
					wpnSkeleton->damage = it->second["damage"].as<int>();
				if (it->second["auto_fire"])
					wpnSkeleton->auto_fire = it->second["auto_fire"].as<bool>();
				if (it->second["bullet_spread"])
					wpnSkeleton->bullet_spread = it->second["bullet_spread"].as<int>();
				if (it->second["weight"])
					wpnSkeleton->weight = it->second["weight"].as<int>();
				if (it->second["ground_image"])
					wpnSkeleton->groundImage = it->second["ground_image"].as<std::string>();
				if (it->second["wear_image"])
					wpnSkeleton->wearImage = it->second["wear_image"].as<std::string>();
				if (it->second["hud_primary"])
					wpnSkeleton->primaryHudIcon = Config::root_node["general"]["hud_path"].as<std::string>() + it->second["hud_primary"].as<std::string>();
				if (it->second["hud_secondary"])
					wpnSkeleton->secondaryHudIcon = Config::root_node["general"]["hud_path"].as<std::string>() + it->second["hud_secondary"].as<std::string>();
				if (it->second["ground_image_size"])
				{
					wpnSkeleton->ground_image_size.x = it->second["ground_image_size"][0].as<float>();
					wpnSkeleton->ground_image_size.y = it->second["ground_image_size"][1].as<float>();
				}
				m_skeletons.insert(std::make_pair(wpnSkeleton->name, wpnSkeleton));
			}
		}
		if (weapons_node["rifles"])
		{
			for (YAML::iterator it = weapons_node["rifles"].begin(); it != weapons_node["rifles"].end(); ++it)
			{
				WeaponSkeleton* wpnSkeleton = new WeaponSkeleton();
				wpnSkeleton->name = it->first.as<std::string>();
				if (it->second["ammo"])
					wpnSkeleton->ammo = it->second["ammo"].as<int>();
				if (it->second["fire_rate"])
					wpnSkeleton->fire_rate = it->second["fire_rate"].as<int>();
				if (it->second["damage"])
					wpnSkeleton->damage = it->second["damage"].as<int>();
				if (it->second["auto_fire"])
					wpnSkeleton->auto_fire = it->second["auto_fire"].as<bool>();
				if (it->second["bullet_spread"])
					wpnSkeleton->bullet_spread = it->second["bullet_spread"].as<int>();
				if (it->second["weight"])
					wpnSkeleton->weight = it->second["weight"].as<int>();
				if (it->second["ground_image"])
					wpnSkeleton->groundImage = it->second["ground_image"].as<std::string>();
				if (it->second["wear_image"])
					wpnSkeleton->wearImage = it->second["wear_image"].as<std::string>();
				if (it->second["hud_primary"])
					wpnSkeleton->primaryHudIcon = Config::root_node["general"]["hud_path"].as<std::string>() + it->second["hud_primary"].as<std::string>();
				if (it->second["hud_secondary"])
					wpnSkeleton->secondaryHudIcon = Config::root_node["general"]["hud_path"].as<std::string>() + it->second["hud_secondary"].as<std::string>();
				if (it->second["ground_image_size"])
				{
					wpnSkeleton->ground_image_size.x = it->second["ground_image_size"][0].as<float>();
					wpnSkeleton->ground_image_size.y = it->second["ground_image_size"][1].as<float>();
				}
				m_skeletons.insert(std::make_pair(wpnSkeleton->name, wpnSkeleton));
			}
		}
	}
}

void WeaponManager::add(Weapon* p_weapon)
{
	m_weapons.push_back(p_weapon);
}

void WeaponManager::setSpritePath(std::string p_value)
{
	m_spritePath = p_value;
}

WeaponSkeleton* WeaponManager::getSkeleton(std::string name)
{
	auto it = m_skeletons.find(name);
	if (it != m_skeletons.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "Failed to create weapon. Skeleton was not found." << std::endl;
		return nullptr;
	}
}
std::string WeaponManager::getSpritePath() {
	return m_spritePath;
}