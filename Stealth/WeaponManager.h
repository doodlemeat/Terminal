#pragma once

#include <map>
#include <vector>
#include "SFML/System/Vector2.hpp"

class Weapon;

struct WeaponSkeleton
{
	WeaponSkeleton();

	std::string name;
	std::string groundImage;
	std::string wearImage;
	std::string primaryHudIcon;
	std::string secondaryHudIcon;
	int ammo;
	int fire_rate;
	int damage;
	int bullet_spread;
	int weight;
	bool auto_fire;
	sf::Vector2f ground_image_size;
};

class WeaponManager
{
public:
	WeaponManager();
	~WeaponManager();

	void configure();
	void add(Weapon* p_weapon);
	void setSpritePath(std::string p_value);
	WeaponSkeleton* getSkeleton(std::string name);
	std::string getSpritePath();
private:
	std::string m_spritePath;
	std::map<std::string, WeaponSkeleton*> m_skeletons;
	std::vector<Weapon*> m_weapons;
};

