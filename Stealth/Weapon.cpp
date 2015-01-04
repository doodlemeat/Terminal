#include "Weapon.h"


Weapon::Weapon()
{
}


Weapon::~Weapon()
{
}

void Weapon::setName(std::string p_value)
{
	m_name = p_value;
}
void Weapon::setAmmo(int p_value)
{
	m_ammo = p_value;
}
void Weapon::setFireRate(int p_value)
{
	m_fire_rate = p_value;
}
void Weapon::setDamage(int p_value)
{
	m_damage = p_value;
}
void Weapon::setBulletSpread(int p_value)
{
	m_bullet_spread = p_value;
}
void Weapon::setWeight(int p_value)
{
	m_weight = p_value;
}
void Weapon::setAutoFire(bool p_value)
{
	m_auto_fire = p_value;
}
void Weapon::setEquipped(bool p_value)
{
	m_equipped = p_value;
}

std::string Weapon::getName()
{
	return m_name;
}
int Weapon::getAmmo()
{
	return m_ammo;
}
int Weapon::getFireRate()
{
	return m_fire_rate;
}
int Weapon::getDamage()
{
	return m_damage;
}
int Weapon::getBulletSpread()
{
	return m_bullet_spread;
}
int Weapon::getWeight()
{
	return m_weight;
}
bool Weapon::getAutoFire()
{
	return m_auto_fire;
}
bool Weapon::getEquppied()
{
	return m_equipped;
}