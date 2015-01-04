#include "GuardStateAttack.h"
#include "Input.h"
#include "Guard.h"
#include "Player.h"
#include "Math.h"

#include <iostream>
#include "Item.h"
#include "Weapon.h"
#include "Audiomanager.h"
#include "BulletManager.h"
#include "Angle.h"

GuardStateAttack::GuardStateAttack()
{
}

bool GuardStateAttack::Init()
{
	m_guard->getAnimatedSprite()->setCurrentAnimation("guard_shoot");
	m_guard->changeOrigin();
	/* If guard has no weapon, call for help (FUTURE) */
	if (m_guard->weapon == nullptr)
	{
		
	}
	return true;
}
void GuardStateAttack::Exit()
{

}

bool GuardStateAttack::Update(sf::Time deltatime)
{
	if (!m_guard->m_light->detectPlayer(m_guard->m_player->m_body->getPosition()))
	{
		setNextState("GuardStateChase");
		return false;
	}

	/* Rotate to the guard to the player */
	m_guard->animatedSprite->setRotation(Math::getAngleFrom(m_guard->m_player->m_body->getPosition(), m_guard->animatedSprite->getPosition()));
	
	if (m_guard->weapon != nullptr && m_guard->weapon->getType() == ITEM_WEAPON)
	{
		Weapon* weapon = static_cast<Weapon*>(m_guard->weapon);
		if (m_guard->m_lastShot.getElapsedTime() > sf::milliseconds(weapon->getFireRate()))
		{
			m_guard->m_lastShot.restart();

			sf::Vector2f bulletSpawnOffset;
			sf::Vector2f bulletPosition;

			/* Get direction of bullet */
			float bulletDirection = Math::getAngleFrom(m_guard->m_player->m_body->getPosition(), m_guard->getAnimatedSprite()->getPosition());

			/* Get the offset of the bullet */
			bulletSpawnOffset.x = 72 * cosf(bulletDirection * Angle::DEG2RAD);
			bulletSpawnOffset.y = 72 * sinf(bulletDirection * Angle::DEG2RAD);

			/* Get the position of the bullet */
			bulletPosition.x = m_guard->getAnimatedSprite()->getPosition().x + bulletSpawnOffset.x;
			bulletPosition.y = m_guard->getAnimatedSprite()->getPosition().y + bulletSpawnOffset.y;
			
			// create a bullet
			Bullet* blt = new Bullet(bulletPosition, bulletDirection);
			blt->setDamage(weapon->getDamage());
			m_guard->m_bulletManager->addBullet(blt);
			Audiomanager::sfx("pang")->play();
		}
	}
	return true;
}

std::string GuardStateAttack::Next()
{
	return next_state;
}
void GuardStateAttack::setNextState(std::string state)
{
	next_state = state;
}
bool GuardStateAttack::IsType(const std::string &type)
{
	return (type == "GuardStateAttack");
}