#include "GuardStateIdle.h"
#include "Input.h"
#include "Guard.h"
#include "Player.h"
#include "SoundrippleManager.h"
#include "Math.h"
#include "Angle.h"
#include <iostream>
#include "GuardState.h"
#include "Pathfinder.h"
#include "SoundRipple.h"

GuardStateIdle::GuardStateIdle()
{

}
bool GuardStateIdle::Init()
{
	m_guard->setSpeed(m_guard->walk_speed);
	m_guard->getAnimatedSprite()->setCurrentAnimation("guard1_walk");
	rotate = false;
	initNewPoint = true;
	return true;
}
void GuardStateIdle::Exit()
{

}

bool GuardStateIdle::Update(sf::Time deltatime)
{
	/* If we have to find a way back to the path */
	if (m_guard->findBack)
	{
		if (fabs(m_guard->hitbox.getPosition().x - m_guard->m_wayBack[m_guard->nextPointWayBack].x) < 5 + m_guard->getSpeed() * deltatime.asSeconds() &&
			fabs(m_guard->hitbox.getPosition().y - m_guard->m_wayBack[m_guard->nextPointWayBack].y) < 5 + m_guard->getSpeed() * deltatime.asSeconds())
		{
			// Find out the next point to go to
			m_guard->nextPointWayBack++;
			initNewPoint = true;
			if (m_guard->nextPointWayBack > static_cast<int>(m_guard->m_wayBack.size() - 1))
			{
				std::cout << "Guard found its way back" << std::endl;
				m_guard->stop();
				initNewPoint = true;
				m_guard->findBack = false;
				m_guard->m_wayBack.clear();
			}
		}

		// setup new point
		if (initNewPoint && m_guard->findBack)
			newPointSetupWayBack();

		// if we can rotate
		if (rotate)
		{
			// choose way to rotate
			if (rotateDirection)
			{
				m_guard->animatedSprite->rotate(2);
			}
			else
			{
				m_guard->animatedSprite->rotate(-2);
			}
		}
		canRotateAgainWayBack();
	}
	else
	{
		if (m_guard->m_patrolPoints.size() > 1)
		{
			// check if guard has reached the next point
			if (fabs(m_guard->hitbox.getPosition().x - m_guard->m_patrolPoints[m_guard->nextPoint].x) < 4 &&
				fabs(m_guard->hitbox.getPosition().y - m_guard->m_patrolPoints[m_guard->nextPoint].y) < 4)
			{
				// Find out the next point to go to
				m_guard->nextPoint++;
				initNewPoint = true;
				if (m_guard->nextPoint > static_cast<int>(m_guard->m_patrolPoints.size() - 1))
					m_guard->nextPoint = 0;
			}

			// setup new point
			if (initNewPoint)
				newPointSetup();

			// if we can rotate
			if (rotate)
			{
				// choose way to rotate
				if (rotateDirection)
				{
					m_guard->animatedSprite->rotate(2);
				}
				else
				{
					m_guard->animatedSprite->rotate(-2);
				}
			}
			canRotateAgain();
		}
	}
	if (m_guard->m_light->detectPlayer(m_guard->m_player->m_body->getPosition()) && m_guard->m_player->m_activeDisg == nullptr)
	{
		m_guard->stop();
		setNextState("GuardStateAttack");
		return false;
	}
	else if (m_guard->m_player->m_soundrippleManager->canPointHearMe(m_guard->animatedSprite->getPosition(), RIPPLE_PLAYER) && m_guard->m_player->m_activeDisg == nullptr)
	{
		m_guard->stop();
		setNextState("GuardStateChase");
		return false;
	}

	SoundRipple sndRipple;
	if (m_guard->m_player->m_soundrippleManager->canPointHearMe(m_guard->animatedSprite->getPosition(), RIPPLE_UNKNOWN, sndRipple))
	{
		/* Calculate a path for the suspicious state */
		m_guard->m_pathfinder->reset();
		m_guard->m_pathfinder->setAllowDiagonalMovement(true);
		m_guard->m_pathfinder->setDontCrossCorners(true);
		sf::Vector2f soundCenter = sndRipple.getCenter();
		m_guard->m_pathfinder->setStartAndGoal(m_guard->m_pathfinder->getTilePosition(m_guard->getAnimatedSprite()->getPosition()), m_guard->m_pathfinder->getTilePosition(soundCenter));
		std::vector<sf::Vector2i> myPoints = m_guard->m_pathfinder->execute();
		if (myPoints.empty())
		{
			return true;
		}
		std::vector<sf::Vector2f> suspPoints;
		sf::Vector2f tileSize = m_guard->m_pathfinder->getTileSize();
		for (unsigned int i = 0; i < myPoints.size(); i++)
		{
			myPoints[i].x *= static_cast<int>(tileSize.x);
			myPoints[i].y *= static_cast<int>(tileSize.y);
			suspPoints.push_back(static_cast<sf::Vector2f>(myPoints[i]));
		}
		m_guard->m_suspPoints = suspPoints;
		m_guard->stop();
		setNextState("GuardStateSuspicious");
		return false;
	}

	return true;
}

std::string GuardStateIdle::Next()
{
	return next_state;
}
void GuardStateIdle::setNextState(std::string state)
{
	next_state = state;
}
bool GuardStateIdle::IsType(const std::string &type)
{
	return (type == "GuardStateIdle");
}

void GuardStateIdle::newPointSetup()
{
	initNewPoint = false;

	// get new velocity
	float dx = m_guard->m_patrolPoints[m_guard->nextPoint].x - m_guard->hitbox.getPosition().x;
	float dy = m_guard->m_patrolPoints[m_guard->nextPoint].y - m_guard->hitbox.getPosition().y;
	float length = sqrtf(dx*dx + dy*dy);
	dx /= length;
	dy /= length;
	m_guard->setVelocityX(dx * m_guard->speed);
	m_guard->setVelocityY(dy * m_guard->speed);

	// get current guard angle
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();
	
	// get angle to next point relative to guard position
	float angle_to_next_point = Math::getAngleFrom(m_guard->m_patrolPoints[m_guard->nextPoint], m_guard->animatedSprite->getPosition());

	// if the difference between the angles is more than 2, we need to rotate it, else it is "almost" rotated correctly (wont happen so often!!)
	if (Math::angle_difference(angle_to_next_point, guard_rotation) > 2)
	{
		rotate = true;
		if (Math::directionOfRotation(guard_rotation, angle_to_next_point))
		{
			rotateDirection = true;
		}
		else
		{
			rotateDirection = false;
		}
	}
	else
		rotate = false;
}
void GuardStateIdle::canRotateAgain()
{
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();
	float angle_to_next_point = Math::getAngleFrom(m_guard->m_patrolPoints[m_guard->nextPoint], m_guard->animatedSprite->getPosition());
	
	if (Math::angle_difference(angle_to_next_point, guard_rotation) < 2)
	{
		rotate = false;
		return;
	}
	rotate = true;
}

void GuardStateIdle::newPointSetupWayBack()
{
	initNewPoint = false;

	// get new velocity
	float dx = m_guard->m_wayBack[m_guard->nextPointWayBack].x - m_guard->hitbox.getPosition().x;
	float dy = m_guard->m_wayBack[m_guard->nextPointWayBack].y - m_guard->hitbox.getPosition().y;
	float length = sqrtf(dx*dx + dy*dy);
	dx /= length;
	dy /= length;
	m_guard->setVelocityX(dx * m_guard->speed);
	m_guard->setVelocityY(dy * m_guard->speed);

	// get current guard angle
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();

	// get angle to next point relative to guard position
	float angle_to_next_point = Math::getAngleFrom(sf::Vector2f(m_guard->m_wayBack[m_guard->nextPointWayBack]), m_guard->animatedSprite->getPosition());

	// if the difference between the angles is more than 2, we need to rotate it, else it is "almost" rotated correctly (wont happen so often!!)
	if (Math::angle_difference(angle_to_next_point, guard_rotation) > 2)
	{
		rotate = true;
		if (Math::directionOfRotation(guard_rotation, angle_to_next_point))
		{
			rotateDirection = true;
		}
		else
		{
			rotateDirection = false;
		}
	}
	else
		rotate = false;
}
void GuardStateIdle::canRotateAgainWayBack()
{
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();
	float angle_to_next_point = Math::getAngleFrom(sf::Vector2f(m_guard->m_wayBack[m_guard->nextPointWayBack]), m_guard->animatedSprite->getPosition());

	if (Math::angle_difference(angle_to_next_point, guard_rotation) < 2)
	{
		rotate = false;
		return;
	}
	rotate = true;
}