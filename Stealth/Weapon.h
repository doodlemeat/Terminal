#pragma once

#include <string>
#include "Item.h"

namespace sf
{
	class Sprite;
}

class Weapon: public Item
{
public:
	Weapon();
	~Weapon();

	void setName(std::string p_value);
	void setAmmo(int p_value);
	void setFireRate(int p_value);
	void setDamage(int p_value);
	void setBulletSpread(int p_value);
	void setWeight(int p_value);
	void setAutoFire(bool p_value);
	void setEquipped(bool p_value);

	std::string getName();
	int getAmmo();
	int getFireRate();
	int getDamage();
	int getBulletSpread();
	int getWeight();
	bool getAutoFire();
	bool getEquppied();

private:
	std::string m_name;
	int m_ammo;
	int m_fire_rate;
	int m_damage;
	int m_bullet_spread;
	int m_weight;
	bool m_auto_fire;
	bool m_equipped;
};

