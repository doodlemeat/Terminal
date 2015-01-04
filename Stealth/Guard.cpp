#include "Guard.h"
#include "Config.h"
#include "LightEngine.h"
#include "Math.h"
#include "Angle.h"
#include "Input.h"
#include "LightEngine.h"
#include "Player.h"
#include "Window.h"
#include "SFML\Graphics\CircleShape.hpp"
#include "Pathfinder.h"
#include "Weapon.h"

#include "GuardStateIdle.h"
#include "GuardStateChase.h"
#include "GuardStateSuspicious.h"
#include "GuardStateAttack.h"
#include <iostream>

Guard::Guard()
{
	rotateTo = 0;
	initNewPoint = true;
	pointSearch = false;
	rotatePositive = true;
	currentPoint = 0;
	nextPoint = 1;
	m_patrolPoints.clear();
	findBack = false;
	int m_lightR = 255;
	int m_lightG = 255;
	int m_lightB = 255;
	int m_lightA = 255;
	weapon = nullptr;
	keycard = nullptr;
	disguise = nullptr;

	GuardStateIdle* gsi = new GuardStateIdle();
	gsi->m_guard = this;
	AddState(gsi);

	GuardStateChase* gsc = new GuardStateChase();
	gsc->m_guard = this;
	AddState(gsc);

	GuardStateSuspicious* gss = new GuardStateSuspicious();
	gss->m_guard = this;
	AddState(gss);

	GuardStateAttack* gsa = new GuardStateAttack();
	gsa->m_guard = this;
	AddState(gsa);
	
	dead = false;
	SetState("GuardStateIdle");
}
Guard::~Guard()
{
	auto it = states.begin();
	while (it != states.end()) {
		delete (*it);
		++it;
	};
	states.clear();
}
void Guard::configure()
{
	hp = 10;
	if (Config::root_node["guards"])
	{
		walk_speed = Config::root_node["guards"]["walk_speed"].as<float>();
		setSpeed(walk_speed);
		run_speed = Config::root_node["guards"]["run_speed"].as<float>();
		m_color = sf::Color(
			Config::root_node["guards"]["vision_r"].as<int>(),
			Config::root_node["guards"]["vision_g"].as<int>(),
			Config::root_node["guards"]["vision_b"].as<int>(),
			Config::root_node["guards"]["vision_a"].as<int>()
			);
		m_color_detected = sf::Color(
			Config::root_node["guards"]["vision_r_detected"].as<int>(),
			Config::root_node["guards"]["vision_g_detected"].as<int>(),
			Config::root_node["guards"]["vision_b_detected"].as<int>(),
			Config::root_node["guards"]["vision_a_detected"].as<int>()
			);
		guard_max_idle_second = Config::root_node["guards"]["guard_max_idle_second"].as<int>();
		hp = Config::root_node["guards"]["hp"].as<int>();
	}
}
void Guard::update(sf::Time deltatime)
{
	if (!dead)
	{
		if (current == nullptr) return;
		if (!current->Update(deltatime) || alerted) {
			ChangeState();
			alerted = false;
		};
	}
	updateHitbox();
	move(deltatime);
	m_light->update(animatedSprite->getPosition(), animatedSprite->getRotation());

	animatedSprite->update(deltatime);
}
void Guard::setWayPoints(std::vector<sf::Vector2f> points)
{
	m_patrolPoints = points;
}
void Guard::createVision(LightEngine* lightEngine)
{
	std::vector<sf::Vector2f> lightPoly;
	lightPoly.emplace_back(0.f, 0.f);
	lightPoly.emplace_back(100.f, -50.f);
	lightPoly.emplace_back(120.f, 0.f);
	lightPoly.emplace_back(100.f, 50.f);
	m_light = lightEngine->addLight(hitbox.getPosition(), lightPoly, 0);
	m_light->m_color = m_color;
	m_light->m_color_detected = m_color_detected;
}
Light* Guard::getLight()
{
	return m_light;
}
void Guard::drawPath()
{
	for (unsigned int i = 0, j = m_patrolPoints.size() - 1; i < m_patrolPoints.size(); j = i++)
	{
		sf::Vector2f p1 = m_patrolPoints[i];
		sf::Vector2f p2 = m_patrolPoints[j];

		sf::Vertex line[] =
		{
			sf::Vertex(p1),
			sf::Vertex(p2)
		};
		line[0].color = sf::Color::Red;
		line[1].color = sf::Color::Red;
		Window::get("main")->draw(line, 2, sf::PrimitiveType::Lines);
	}

	for (unsigned int i = 0, j = m_chasePoints.size() - 1; i < m_chasePoints.size(); j = i++)
	{
		sf::Vector2f p1 = m_chasePoints[i];
		sf::Vector2f p2 = m_chasePoints[j];

		sf::Vertex line[] =
		{
			sf::Vertex(p1),
			sf::Vertex(p2)
		};
		line[0].color = sf::Color::Black;
		line[1].color = sf::Color::Black;
		Window::get("main")->draw(line, 2, sf::PrimitiveType::Lines);
	}

	for (auto point : m_chasePoints)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(30, 30));
		rect.setFillColor(sf::Color::Red);
		sf::Vector2f tileSize = m_pathfinder->getTileSize();
		rect.setPosition(point.x, point.y);
		Window::get("main")->draw(rect);
	}

	for (auto point : m_wayBack)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(15, 15));
		rect.setFillColor(sf::Color::Red);
		sf::Vector2f tileSize = m_pathfinder->getTileSize();
		rect.setPosition(point.x, point.y);
		Window::get("main")->draw(rect);
	}

	for (auto point : m_suspPoints)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(30, 30));
		rect.setFillColor(sf::Color::Red);
		sf::Vector2f tileSize = m_pathfinder->getTileSize();
		rect.setPosition(point.x, point.y);
		Window::get("main")->draw(rect);
	}

	sf::CircleShape shape;
	shape.setRadius(10);
	shape.setOrigin(10, 10);
	shape.setFillColor(sf::Color::Blue);
	shape.setPosition(m_patrolPoints[nextPoint]);
	Window::get("main")->draw(shape);
}
void Guard::setPathfinder(Pathfinder* p_pathfinder)
{
	m_pathfinder = p_pathfinder;
}
void Guard::setWeapon(Item* p_item)
{
	weapon = p_item;
	sf::Vector2f weaponPosition;
	weaponPosition.x = 128;
	weaponPosition.y = 64;
	weapon->getWearSprite()->setPosition(weaponPosition);
}
void Guard::setKeycard(Item* p_item)
{
	keycard = p_item;
}
void Guard::setDisguise(Item* p_item)
{
	disguise = p_item;
}
void Guard::setBulletManager(BulletManager* p_bulletManager)
{
	m_bulletManager = p_bulletManager;
}
void Guard::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*animatedSprite, states);
	if (weapon != nullptr && current->IsType("GuardStateAttack"))
	{
		states.transform = animatedSprite->getTransform();
		target.draw(*weapon->getWearSprite(), states);
	}
}
void Guard::changeOrigin()
{
	getAnimatedSprite()->setOrigin(
		getAnimatedSprite()->getGlobalBounds().width / 2,
		getAnimatedSprite()->getGlobalBounds().height / 2);

	/* Change light position */
	m_light->changePosition(animatedSprite->getPosition());
}
void Guard::changeHP(int p_value)
{
	hp += p_value;
	if (hp <= 0)
	{
		stop();
		dead = true;
		getAnimatedSprite()->setLoopable(false);
		getAnimatedSprite()->setCurrentAnimation("guard1_die1");
		m_light->setDead(true);
		if (weapon != nullptr)
		{
			// Drop weapon
			weapon->setWear(false);
			weapon->getGroundSprite()->setPosition(animatedSprite->getPosition());
			weapon = nullptr;
		}
	}
}
void Guard::alert()
{
	m_pathfinder->reset();
	m_pathfinder->setAllowDiagonalMovement(true);
	m_pathfinder->setDontCrossCorners(true);
	m_pathfinder->setStartAndGoal(m_pathfinder->getTilePosition(getAnimatedSprite()->getPosition()), m_pathfinder->getTilePosition(m_player->m_body->getPosition()));
	std::vector<sf::Vector2i> myPoints = m_pathfinder->execute();
	std::vector<sf::Vector2f> chasePoints;
	sf::Vector2f tileSize = m_pathfinder->getTileSize();
	for (unsigned int i = 0; i < myPoints.size(); i++)
	{
		myPoints[i].x *= static_cast<int>(tileSize.x);
		myPoints[i].y *= static_cast<int>(tileSize.y);
		chasePoints.push_back(static_cast<sf::Vector2f>(myPoints[i]));
	}
	m_chasePoints = chasePoints;
	current->setNextState("GuardStateChase");
	alerted = true;
}

void Guard::AddState(GuardState* state)
{
	states.push_back(state);
}
void Guard::ChangeState()
{
	std::string next = current->Next();
	if (current != nullptr) {
		current->Exit();
		current = nullptr;
	};
	if (next.empty()) {
		return;
	}

	for (unsigned int i = 0; i < states.size(); i++) {
		if (states[i]->IsType(next)) {
			current = states[i];
			current->Init();
			return;
		};
	};
}
void Guard::SetState(std::string type)
{
	for (unsigned int i = 0; i < states.size(); i++) {
		if (states[i]->IsType(type)) {
			current = states[i];
			current->Init();
		};
	};
}

void Guard::setCameraDistance(float distance)
{
	m_cameraDistance = distance;
}

float Guard::getCameraDistance()
{
	return m_cameraDistance;
}
