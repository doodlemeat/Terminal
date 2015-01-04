#include "GuardStateChase.h"
#include <iostream>
#include "Guard.h"
#include "Pathfinder.h"
#include "Player.h"
#include "Math.h"

GuardStateChase::GuardStateChase()
{

}
bool GuardStateChase::Init()
{
	m_guard->setSpeed(m_guard->run_speed);
	m_guard->getAnimatedSprite()->setCurrentAnimation("guard1_run");
	find_new_path = true;
	initNewPoint = true;
	nextPoint = 0;

	rotateDirection = false;
	rotate = false;
	timer.restart();

	return true;
}
void GuardStateChase::Exit()
{

}

bool GuardStateChase::Update(sf::Time deltatime)
{
	if (timer.getElapsedTime().asSeconds() >= m_guard->guard_max_idle_second)
	{
		/* Calculate path to the next point in the walkway */
		m_guard->m_pathfinder->reset();
		m_guard->m_pathfinder->setAllowDiagonalMovement(true);
		m_guard->m_pathfinder->setDontCrossCorners(true);
		m_guard->m_pathfinder->setStartAndGoal(m_guard->m_pathfinder->getTilePosition(m_guard->getAnimatedSprite()->getPosition()), m_guard->m_pathfinder->getTilePosition(m_guard->m_patrolPoints[m_guard->nextPoint]));
		m_guard->m_wayBack = m_guard->m_pathfinder->execute();
		sf::Vector2f tileSize = m_guard->m_pathfinder->getTileSize();
		for (unsigned int i = 0; i < m_guard->m_wayBack.size(); i++)
		{
			m_guard->m_wayBack[i].x *= tileSize.x;
			m_guard->m_wayBack[i].y *= tileSize.y;
		}	
		m_guard->stop();
		m_guard->setSpeed(m_guard->walk_speed);
		m_guard->findBack = true;
		m_guard->nextPointWayBack = 0;
		m_guard->m_chasePoints.clear();
		setNextState("GuardStateIdle");
		return false;
	}
	if (!m_guard->m_light->detectPlayer(m_guard->m_player->m_body->getPosition()))
	{
		if (find_new_path)
		{
			/* Calculate a new path */
			find_new_path = false;
			m_guard->m_pathfinder->reset();
			m_guard->m_pathfinder->setAllowDiagonalMovement(true);
			m_guard->m_pathfinder->setDontCrossCorners(true);
			m_guard->m_pathfinder->setStartAndGoal(m_guard->m_pathfinder->getTilePosition(m_guard->getAnimatedSprite()->getPosition()), m_guard->m_pathfinder->getTilePosition(m_guard->m_player->m_body->getPosition()));
			std::vector<sf::Vector2i> myPoints = m_guard->m_pathfinder->execute();
			std::vector<sf::Vector2f> chasePoints;
			sf::Vector2f tileSize = m_guard->m_pathfinder->getTileSize();
			for (unsigned int i = 0; i < myPoints.size(); i++)
			{
				myPoints[i].x *= static_cast<int>(tileSize.x);
				myPoints[i].y *= static_cast<int>(tileSize.y);
				chasePoints.push_back(static_cast<sf::Vector2f>(myPoints[i]));
			}
			m_guard->m_chasePoints = chasePoints;
		}
		else
		{
			if (m_guard->m_chasePoints.size() > 1)
			{
				/* Check if we reached the next point */
				if (fabs(m_guard->hitbox.getPosition().x - m_guard->m_chasePoints[nextPoint].x) < 11 &&
					fabs(m_guard->hitbox.getPosition().y - m_guard->m_chasePoints[nextPoint].y) < 11)
				{
					// Find out the next point to go to
					nextPoint++;
					initNewPoint = true;

					if (nextPoint == static_cast<int>(m_guard->m_chasePoints.size()))
					{
						std::cout << "I found the last node in my path!" << std::endl;
						nextPoint = 0;
						m_guard->m_chasePoints.clear();
						m_guard->stop();
						find_new_path = true;
						rotateDirection = false;
						rotate = false;
					}
				}

				// setup new point
				if (initNewPoint && !find_new_path)
					newPointSetup();

				// if we can rotate
				if (rotate)
				{
					// choose way to rotate
					if (rotateDirection)
					{
						m_guard->animatedSprite->rotate(5);
					}
					else
					{
						m_guard->animatedSprite->rotate(-5);
					}
				}
				canRotateAgain();
			}
		}
	}
	else
	{
		/* The guard sees the player, go to attack state */
		m_guard->stop();
		m_guard->m_chasePoints.clear();
		setNextState("GuardStateAttack");
		return false;
	}
	return true;
}

void GuardStateChase::newPointSetup()
{
	initNewPoint = false;

	// get new velocity
	float dx = m_guard->m_chasePoints[nextPoint].x - m_guard->hitbox.getPosition().x;
	float dy = m_guard->m_chasePoints[nextPoint].y - m_guard->hitbox.getPosition().y;
	float length = sqrtf(dx*dx + dy*dy);
	dx /= length;
	dy /= length;
	m_guard->setVelocityX(dx * m_guard->speed);
	m_guard->setVelocityY(dy * m_guard->speed);


	// get current guard angle
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();

	// get angle to next point relative to guard position
	float angle_to_next_point = Math::getAngleFrom(m_guard->m_chasePoints[nextPoint], m_guard->animatedSprite->getPosition());

	// if the difference between the angles is more than 2, we need to rotate it, else it is "almost" rotated correctly (wont happen so often!!)
	if (Math::angle_difference(angle_to_next_point, guard_rotation) > 5)
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

void GuardStateChase::canRotateAgain()
{
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();
	float angle_to_next_point = Math::getAngleFrom(m_guard->m_chasePoints[nextPoint], m_guard->animatedSprite->getPosition());

	if (Math::angle_difference(angle_to_next_point, guard_rotation) < 2)
	{
		rotate = false;
		return;
	}
	rotate = true;
}

std::string GuardStateChase::Next()
{
	return next_state;
}
void GuardStateChase::setNextState(std::string state)
{
	next_state = state;
}
bool GuardStateChase::IsType(const std::string &type)
{
	return (type == "GuardStateChase");
}