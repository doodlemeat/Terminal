#include "GuardStateSuspicious.h"
#include "GuardState.h"
#include "Guard.h"
#include "Player.h"
#include "Math.h"
#include "Pathfinder.h"

GuardStateSuspicious::GuardStateSuspicious()
{

}
bool GuardStateSuspicious::Init()
{
	m_guard->setSpeed(m_guard->walk_speed);
	m_guard->getAnimatedSprite()->setCurrentAnimation("guard1_walk");
	find_new_path = false;
	initNewPoint = true;
	rotateDirection = false;
	rotate = false;
	wait = false;
	give_up = false;
	start_rotation = 0;
	rotated_degrees = 0;
	return true;
}
void GuardStateSuspicious::Exit()
{

}

bool GuardStateSuspicious::Update(sf::Time deltatime)
{
	if (m_guard->m_light->detectPlayer(m_guard->m_player->m_body->getPosition()))
	{
		m_guard->stop();
		m_guard->m_chasePoints.clear();
		setNextState("GuardStateAttack");
		return false;
	}
	if (wait)
	{
		if (rotated_degrees >= 360)
		{
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
			setNextState("GuardStateIdle");
			return false;
		}

		m_guard->animatedSprite->rotate(1);
		rotated_degrees += 1;
	}
	if (m_guard->m_suspPoints.size() > 1)
	{
		/* Check if we reached the next point */
		if (fabs(m_guard->hitbox.getPosition().x - m_guard->m_suspPoints[m_guard->nextPointSusp].x) < 11 &&
			fabs(m_guard->hitbox.getPosition().y - m_guard->m_suspPoints[m_guard->nextPointSusp].y) < 11)
		{
			// Find out the next point to go to
			m_guard->nextPointSusp++;
			initNewPoint = true;

			if (m_guard->nextPointSusp == static_cast<int>(m_guard->m_suspPoints.size()))
			{
				m_guard->getAnimatedSprite()->setCurrentAnimation("guard1_walk");
				start_rotation = m_guard->animatedSprite->getRotation();
				wait = true;
				m_guard->nextPointSusp = 0;
				m_guard->m_suspPoints.clear();
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
				m_guard->animatedSprite->rotate(3);
			}
			else
			{
				m_guard->animatedSprite->rotate(-3);
			}
		}
		canRotateAgain();
	}
	return true;
}

std::string GuardStateSuspicious::Next()
{
	return next_state;
}
void GuardStateSuspicious::setNextState(std::string state)
{
	next_state = state;
}
bool GuardStateSuspicious::IsType(const std::string &type)
{
	return (type == "GuardStateSuspicious");
}

void GuardStateSuspicious::newPointSetup()
{
	initNewPoint = false;

	// get new velocity
	float dx = m_guard->m_suspPoints[m_guard->nextPointSusp].x - m_guard->hitbox.getPosition().x;
	float dy = m_guard->m_suspPoints[m_guard->nextPointSusp].y - m_guard->hitbox.getPosition().y;
	float length = sqrtf(dx*dx + dy*dy);
	dx /= length;
	dy /= length;
	m_guard->setVelocityX(dx * m_guard->speed);
	m_guard->setVelocityY(dy * m_guard->speed);


	// get current guard angle
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();

	// get angle to next point relative to guard position
	float angle_to_next_point = Math::getAngleFrom(m_guard->m_suspPoints[m_guard->nextPointSusp], m_guard->animatedSprite->getPosition());

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

void GuardStateSuspicious::canRotateAgain()
{
	float guard_rotation = m_guard->getAnimatedSprite()->getRotation();
	float angle_to_next_point = Math::getAngleFrom(m_guard->m_suspPoints[m_guard->nextPointSusp], m_guard->animatedSprite->getPosition());

	if (Math::angle_difference(angle_to_next_point, guard_rotation) < 2)
	{
		rotate = false;
		return;
	}
	rotate = true;
}
