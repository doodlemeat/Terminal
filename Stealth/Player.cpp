#include "Player.h"
#include "Config.h"
#include "Audiomanager.h"
#include "Window.h"
#include "Input.h"
#include "Math.h"
#include "Angle.h"
#include "SoundrippleManager.h"
#include "SpriteManager.h"
#include "Item.h"
#include "Weapon.h"
#include "SFML/Graphics/Sprite.hpp"
#include "BulletManager.h"
#include "Goal.h"
#include <iostream>
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Audio/Sound.hpp"
#include "Thor/Math/Random.hpp"
#include "Thor/Input.hpp"
#include "Coin.h"
#include "ItemManager.h"
#include "Disg.h"

namespace
{
	inline sf::Uint8 lerpUint8(sf::Uint8 a, sf::Uint8 b, float alpha)
	{
		return static_cast<sf::Uint8>(Math::clamp(static_cast<float>(a)+(static_cast<float>(b)-static_cast<float>(a)) * alpha, 0.f, 255.f));
	}
	inline sf::Color lerpColor(const sf::Color &colorA, const sf::Color &colorB, float alpha)
	{
		return sf::Color(
			lerpUint8(colorA.r, colorB.r, alpha),
			lerpUint8(colorA.g, colorB.g, alpha),
			lerpUint8(colorA.b, colorB.b, alpha),
			lerpUint8(colorA.a, colorB.a, alpha));
	}
}

Player::Player()
{
	m_viewport = nullptr;
	m_hat = new AnimatedSprite();
	m_head = new AnimatedSprite();
	m_body = new AnimatedSprite();
	m_cloth = new AnimatedSprite();
	m_blood = new AnimatedSprite();

	m_char = new sf::Sprite();
	m_charBackgroundHud = new sf::Sprite();
	m_charFrameHud = new sf::Sprite();

	move_type = PLAYER_IDLE;
	m_currentItem = nullptr;
	m_secondaryItem = nullptr;
	m_activeDisg = nullptr;

	fatigue_bar = new sf::RectangleShape(sf::Vector2f(100, 6));
	fatigue_bar->setOrigin(fatigue_bar->getLocalBounds().width / 2, fatigue_bar->getLocalBounds().height / 2);
	fatigue_bar->setFillColor(sf::Color(8, 161, 254, 100));

	fatigue_bar_background = new sf::RectangleShape(sf::Vector2f(100, 10));
	fatigue_bar_background->setFillColor(sf::Color(51, 51, 51, 180));
	fatigue_bar_background->setOrigin(fatigue_bar_background->getLocalBounds().width / 2, fatigue_bar_background->getLocalBounds().height / 2);

	m_canShoot = true;
	m_levelFinished = false;
	m_hasThrownCoin = false;
	speed = 1;
}
Player::~Player()
{
	delete m_hat;
	m_hat = nullptr;
	delete m_head;
	m_head = nullptr;
	delete m_body;
	m_body = nullptr;
	delete m_cloth;
	m_cloth = nullptr;
	delete m_viewport;
	m_viewport = nullptr;
	delete fatigue_bar;
	fatigue_bar = nullptr;
	delete m_char;
	m_char = nullptr;
	delete m_charBackgroundHud;
	m_charBackgroundHud = nullptr;
	delete m_charFrameHud;
	m_charFrameHud = nullptr;
	Input::clearCallbacks();
}
void Player::setMoveType(int type, sf::Time deltatime, bool or)
{
	if (or || type != move_type)
	{
		move_type = type;
		switch (move_type)
		{
		case PLAYER_SNEAK: onSneak(deltatime); break;
		case PLAYER_SPRINT: onSprint(deltatime); break;
		case PLAYER_WALK: onWalk(deltatime); break;
		case PLAYER_IDLE: onIdle(deltatime); break;
		}
	}
}
void Player::update(std::vector<Goal*> goals, sf::Time deltatime)
{
	if (isDead())
	{
		m_blood->update(deltatime);
		m_body->update(deltatime);
		return;
	}
	if (moving())
	{
		if (sneaking())
		{
			setMoveType(PLAYER_SNEAK, deltatime);
		}
		else if (sprinting())
		{
			setMoveType(PLAYER_SPRINT, deltatime);
		}
		else
		{
			setMoveType(PLAYER_WALK, deltatime);
		}
	}
	else
	{
		setMoveType(PLAYER_IDLE, deltatime);
	}

	switch (move_type)
	{
	case PLAYER_SNEAK:
		whenSneak(deltatime);
		break;
	case PLAYER_WALK:
		whenWalk(deltatime);
		break;
	case PLAYER_SPRINT:
		whenSprint(deltatime);
		break;
	case PLAYER_IDLE:
		whenIdle(deltatime);
		break;
	}

	/* Check if we switch items */
	if (m_secondaryItem != nullptr && (Input::pressed("mouse_scroll_down") || Input::pressed("mouse_scroll_up")))
	{
		std::swap(m_currentItem, m_secondaryItem);
	}

	/* Throw item */
	if (Input::pressed_once(key_dropitem))
	{
		if (m_currentItem != nullptr)
		{
			m_currentItem->setWear(false);
			m_currentItem->setCanBePickedUp(true);
			m_currentItem->getGroundSprite()->setPosition(m_body->getPosition());
			m_currentItem = nullptr;
			if (m_secondaryItem != nullptr)
			{
				std::swap(m_currentItem, m_secondaryItem);
				m_currentItem->setWear(true);
			}
			setMoveType(move_type, deltatime, true);
		}
	}
	
	/* Change position of body */
	m_body->setPosition(sf::Vector2f(
		hitbox.getPosition().x,
		hitbox.getPosition().y));


	sf::Vector2f mouse_position = Window::get("main")->getMousePosition();
	float rotation_to_mouse = atan2f(
		-m_body->getPosition().y - -mouse_position.y,
		-m_body->getPosition().x - -mouse_position.x);
	stop();
	bool moved = false;
	if (Input::pressed(key_left)) {
		velocity.x = -1.f * getSpeed();
		moved = true;
	}
	if (Input::pressed(key_right)) { 
		velocity.x = 1.f * getSpeed();
		moved = true;
	}
	if (Input::pressed(key_up)) { 
		velocity.y = -1.f * getSpeed();
		moved = true;
	}
	if (Input::pressed(key_down)) { 
		velocity.y = 1.f * getSpeed();
		moved = true;
	}

	m_head->setRotation(rotation_to_mouse * Angle::RAD2DEG);
	m_hat->setRotation(rotation_to_mouse * Angle::RAD2DEG);
	m_body->setRotation(rotation_to_mouse * Angle::RAD2DEG);

	// Updates disguise
	if (m_activeDisg != nullptr)
	{
		// Has the active disguise been worn in more than X seconds
		if (m_activeDisg->getTimer()->getElapsedTime().asSeconds() >= m_disguiseTimer)
		{
			// Delete the item from the whole game
			m_itemManager->deleteItem(m_activeDisg);
			m_activeDisg = nullptr;

			// Change animation
			m_hat->setCurrentAnimation("empty");
		}
	}

	// Handle disguise
	if (m_currentItem != nullptr && m_currentItem->getType() == ITEM_DISG)
	{
		if (m_canShoot && m_activeDisg == nullptr && Input::pressed_once(key_shoot))
		{
			m_canShoot = false;
			m_activeDisg = static_cast<Disg*>(m_currentItem);
			m_activeDisg->getTimer()->restart();
			m_currentItem = nullptr;
			if (m_secondaryItem != nullptr)
			{
				std::swap(m_currentItem, m_secondaryItem);
			}
			m_hat->setCurrentAnimation("player_hat_disg");
		}
	}
	if (m_currentItem != nullptr && m_currentItem->getType() == ITEM_WEAPON)
	{
		Weapon* weapon = static_cast<Weapon*>(m_currentItem);

		/* Fire a bullet */
		if (m_canShoot)
		{
			bool fired = false;
			if (weapon->getAutoFire())
			{
				// AUTO fire
				if (Input::pressed(key_shoot) && m_lastShot.getElapsedTime() > sf::milliseconds(weapon->getFireRate()))
				{
					fired = true;
				}
			}
			else
			{
				if (Input::pressed_once(key_shoot) && m_lastShot.getElapsedTime() > sf::milliseconds(weapon->getFireRate()))
				{
					fired = true;
				}
			}

			if (fired)
			{
				if (weapon->getAmmo() == 0)
				{
					if (m_lastClipEmptySound.getElapsedTime() > sf::milliseconds(emptyClipTimer))
					{
						m_lastClipEmptySound.restart();
						Audiomanager::sfx("ammo_empty")->play();
					}
				}
				else
				{
					if (weapon->getAutoFire())
						Audiomanager::sfx("rifle")->play();
					else
						Audiomanager::sfx("pang")->play();
					// If she shoots, the disguise will be removed instantly
					if (m_activeDisg != nullptr)
					{
						m_itemManager->deleteItem(m_activeDisg);
						m_activeDisg = nullptr;
						m_hat->setCurrentAnimation("empty");
					}

					// Updates the fatigue every shoot
					changeFatigue(fatique_change_shoot);

					sf::Vector2f mousePosition = Window::get("main")->getMousePosition();
					sf::Vector2f bulletSpawnOffset;
					sf::Vector2f bulletPosition;

					/* Get direction of bullet */
					float bulletDirection = Math::getAngleFrom(mousePosition, m_body->getPosition());

					/* Get the offset of the bullet */
					bulletSpawnOffset.x = 72 * cosf(bulletDirection * Angle::DEG2RAD);
					bulletSpawnOffset.y = 72 * sinf(bulletDirection * Angle::DEG2RAD);

					/* Get the position of the bullet */
					bulletPosition.x = m_body->getPosition().x + bulletSpawnOffset.x;
					bulletPosition.y = m_body->getPosition().y + bulletSpawnOffset.y;

					/*Set a random shooting angle with fatigue as a variable */

					if (fatigue >= 50){
						if (fatigue >= 75){
							max_direction = bulletDirection + weapon->getBulletSpread();
							min_direction = bulletDirection - weapon->getBulletSpread();
						}
						else{

							max_direction = bulletDirection + (weapon->getBulletSpread()/2);
							min_direction = bulletDirection - (weapon->getBulletSpread()/2);
						}
					}
					else if (fatigue < 50){
						if (fatigue >= 25){
							max_direction = bulletDirection + (weapon->getBulletSpread()/3);
							min_direction = bulletDirection - (weapon->getBulletSpread()/3);
						}
						else{
							max_direction = bulletDirection + (weapon->getBulletSpread()/4);
							min_direction = bulletDirection - (weapon->getBulletSpread()/4);
						}
					}


					if (max_direction > min_direction){
						bulletDirection = rand() % (max_direction - min_direction) + min_direction;
					}
					else if (max_direction < min_direction){
						bulletDirection = rand() % (min_direction - max_direction) + max_direction;
					}

					Bullet* blt = new Bullet(bulletPosition, bulletDirection);
					blt->setDamage(weapon->getDamage());
					m_bulletManager->addBullet(blt);
					m_soundrippleManager->createRipple(m_body->getPosition(), static_cast<float>(shoot_radius), RIPPLE_PLAYER);
					weapon->setAmmo(weapon->getAmmo() - 1);
					m_lastShot.restart();
				}
			}
		}
		playerPositionWeapon();
		
	}
	else if (m_canShoot && m_currentItem != nullptr && m_currentItem->getType() == ITEM_COIN)
	{
		if (Input::pressed(key_shoot))
		{
			m_hasThrownCoin = true;
			m_canShoot = false;
			sf::Vector2f start = m_body->getPosition();
			sf::Vector2f destination = Window::get("main")->getMousePosition();

			Coin* coin = static_cast<Coin*>(m_currentItem);

			float dx = destination.x - start.x;
			float dy = destination.y - start.y;
			float length = sqrtf(dx*dx + dy*dy);
			dx /= length;
			dy /= length;
			coin->setVelocityX(dx * m_itemManager->getCoinSpeed());
			coin->setVelocityY(dy * m_itemManager->getCoinSpeed());
			coin->setWear(false);
			coin->getGroundSprite()->setPosition(m_body->getPosition());
			coin->setCanBePickedUp(true);
			coin->setDestination(destination);
			m_currentItem = nullptr;
			if (m_secondaryItem != nullptr)
				std::swap(m_currentItem, m_secondaryItem);
		}
	}
	
	if (m_canShoot && m_currentItem != nullptr && m_currentItem->getType() == ITEM_KEYCARD)
	{
		if (Input::pressed_once(key_shoot))
		{
			m_canShoot = false;
			for (auto &goal : goals)
			{
				if (goal->getArea()->intersects(hitbox.getGlobalBounds()))
				{
					m_levelFinished = true;
				}
			}
		}
	}

	move(deltatime);

	int volume = ((static_cast<float>(fatigue)-static_cast<float>(min_fatigue)) / max_fatigue) * 100;
	
	if (volume > 0 && m_lastSoundEmitBreath.getElapsedTime().asMilliseconds() > sound_emit_breath_timer)
	{
		float radius = static_cast<float>(volume)* sound_emit_breath_radius_factor;
		m_soundrippleManager->createRipple(
			m_body->getPosition(),
			static_cast<float>(volume)* sound_emit_breath_radius_factor
			, RIPPLE_PLAYER);
		m_lastSoundEmitBreath.restart();
	}
	Audiomanager::sfx("breath")->setVolume(volume);
	
	m_blood->update(deltatime);
	m_hat->update(deltatime);
	m_body->update(deltatime);
	m_cloth->update(deltatime);
	m_head->update(deltatime);
}
void Player::configure()
{
	// Setup action map
	if (Config::root_node["player"])
	{
		this->walkSounds.clear();
		this->sprintSounds.clear();
		this->sneakSounds.clear();
		for (unsigned int i = 0; i < Config::root_node["sounds"]["player_walk"].size(); i++)
		{
			this->walkSounds.push_back(Config::root_node["sounds"]["player_walk"][i].as<std::string>());
		}
		for (unsigned int i = 0; i < Config::root_node["sounds"]["player_sprint"].size(); i++)
		{
			this->sprintSounds.push_back(Config::root_node["sounds"]["player_sprint"][i].as<std::string>());
		}
		for (unsigned int i = 0; i < Config::root_node["sounds"]["player_sneak"].size(); i++)
		{
			this->sneakSounds.push_back(Config::root_node["sounds"]["player_sneak"][i].as<std::string>());
		}
		this->key_up = Config::root_node["player"]["key_up"].as<std::string>();
		this->key_down = Config::root_node["player"]["key_down"].as<std::string>();
		this->key_left = Config::root_node["player"]["key_left"].as<std::string>();
		this->key_right = Config::root_node["player"]["key_right"].as<std::string>();
		this->key_activate_sprint = Config::root_node["player"]["key_activate_sprint"].as<std::string>();
		this->key_activate_sneak = Config::root_node["player"]["key_activate_sneak"].as<std::string>();
		this->key_toggle_items = Config::root_node["player"]["key_toggle_items"].as<std::string>();
		this->key_use = Config::root_node["player"]["key_use"].as<std::string>();
		this->key_shoot = Config::root_node["player"]["key_shoot"].as<std::string>();
		this->key_useaction = Config::root_node["player"]["key_useaction"].as<std::string>();
		this->key_dropitem = Config::root_node["player"]["key_dropitem"].as<std::string>();

		this->sprint_speed = Config::root_node["player"]["sprint_speed"].as<float>();
		this->walk_speed = Config::root_node["player"]["walk_speed"].as<float>();
		this->sneak_speed = Config::root_node["player"]["sneak_speed"].as<float>();
		this->sprint_slowdown_factor = Config::root_node["player"]["sprint_slowdown_factor"].as<float>();
		this->walk_slowdown_factor = Config::root_node["player"]["walk_slowdown_factor"].as<float>();
		this->sneak_slowdown_factor = Config::root_node["player"]["sneak_slowdown_factor"].as<float>();
		this->sound_emit_breath_radius_factor = Config::root_node["player"]["sound_emit_breath_radius_factor"].as<float>();
		m_disguiseTimer = Config::root_node["player"]["disguise_timer"].as<float>();

		this->emptyClipTimer = Config::root_node["general"]["empty_clip_timer"].as<int>();
		this->max_hp = Config::root_node["player"]["max_hp"].as<int>();
		this->hp = Config::root_node["player"]["hp"].as<int>();
		this->min_fatigue = Config::root_node["player"]["min_fatigue"].as<int>();
		this->max_fatigue = Config::root_node["player"]["max_fatigue"].as<int>();
		this->fatigue = min_fatigue;
		this->player_sneak_rr = Config::root_node["sounds"]["player_sneak_rr"].as<int>();
		this->player_walk_rr = Config::root_node["sounds"]["player_walk_rr"].as<int>();
		this->player_sprint_rr = Config::root_node["sounds"]["player_sprint_rr"].as<int>();
		this->shoot_radius = Config::root_node["player"]["shoot_radius"].as<int>();
		this->step_radius = Config::root_node["player"]["step_radius"].as<int>();
		this->fatique_change_shoot = Config::root_node["player"]["fatique_change_shoot"].as<int>();
		this->fatigue_change_idle = Config::root_node["player"]["fatigue_change_idle"].as<int>();
		this->fatigue_change_idle_timer = Config::root_node["player"]["fatigue_change_idle_timer"].as<int>();
		this->fatigue_change_sneak = Config::root_node["player"]["fatigue_change_sneak"].as<int>();
		this->fatigue_change_sneak_timer = Config::root_node["player"]["fatigue_change_sneak_timer"].as<int>();
		this->fatigue_change_walk = Config::root_node["player"]["fatigue_change_walk"].as<int>();
		this->fatigue_change_walk_timer = Config::root_node["player"]["fatigue_change_walk_timer"].as<int>();
		this->fatigue_change_sprint = Config::root_node["player"]["fatigue_change_sprint"].as<int>();
		this->fatigue_change_sprint_timer = Config::root_node["player"]["fatigue_change_sprint_timer"].as<int>();
		this->sound_emit_move_timer = Config::root_node["player"]["sound_emit_move_timer"].as<int>();
		this->sound_emit_breath_timer = Config::root_node["player"]["sound_emit_breath_timer"].as<int>();
		this->sneak_soundripple_radius = Config::root_node["player"]["sneak_soundripple_radius"].as<int>();
		this->walk_soundripple_radius = Config::root_node["player"]["walk_soundripple_radius"].as<int>();
		this->sprint_soundripple_radius = Config::root_node["player"]["sprint_soundripple_radius"].as<int>();
	}
	else
	{
		std::cout << "Failed to configure player. Key not found" << std::endl;
	}

	m_charPortraits.clear();
	int current_hp = 0;
	for (int i = 0; i < Config::root_node["hud"]["char"]["images"].size(); i++)
	{
		std::string filename = Config::root_node["hud"]["char"]["images"][i].as<std::string>();
		SpriteManager::loadImage(filename);
		m_charPortraits.insert(std::make_pair(current_hp, filename));
		current_hp++;
	}

	float myCharFrameScale = Config::root_node["hud"]["char_frame"]["scale"].as<float>();

	m_char->setPosition(Config::root_node["hud"]["char"]["x"].as<float>(), Config::root_node["hud"]["char"]["y"].as<float>());
	m_char->setTexture(*SpriteManager::loadImage(m_charPortraits[hp - 1]));

	m_charBackgroundHud->setPosition(Config::root_node["hud"]["char_frame"]["x"].as<float>(), Config::root_node["hud"]["char_frame"]["y"].as<float>());
	m_charBackgroundHud->setTexture(*SpriteManager::loadImage("hud/char_bg.png"));
	m_charBackgroundHud->setScale(myCharFrameScale, myCharFrameScale);

	m_charFrameHud->setTexture(*SpriteManager::loadImage("hud/char_frame.png"));
	m_charFrameHud->setScale(myCharFrameScale, myCharFrameScale);
	m_charFrameHud->setPosition(Config::root_node["hud"]["char_frame"]["x"].as<float>(), Config::root_node["hud"]["char_frame"]["y"].as<float>());

	Input::onMouseUp(key_shoot, std::bind(&Player::handleReleaseShoot, this, std::placeholders::_1));
}
void Player::setViewport(float width, float height, float maxX, float maxY)
{
	m_viewport = new Viewport(width, height);
	m_viewport->setMax(sf::Vector2f(maxX, maxY));
}
void Player::updateViewport()
{
	m_viewport->update(m_body->getPosition());
	m_viewport->getView()->setCenter(m_body->getPosition().x, m_body->getPosition().y);
}
void Player::setSoundrippleManager(SoundrippleManager* soundrippleManager)
{
	m_soundrippleManager = soundrippleManager;
}
void Player::setBulletManager(BulletManager* p_bulletManager)
{
	m_bulletManager = p_bulletManager;
}
void Player::changeFatigue(int p_fatique_change)
{
	fatigue += p_fatique_change;
	fatigue = Math::clamp(fatigue, 0, max_fatigue);
}
void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// blod
	sf::Transform blood_transform = m_body->getTransform();

	blood_transform.translate(sf::Vector2f(
		m_body->getLocalBounds().width / 2,
		m_body->getLocalBounds().height / 2
		));
	target.draw(*m_blood, blood_transform.rotate(-m_body->getRotation()));

	// draw body
	target.draw(*m_body, states);

	if (!m_dead)
	{
		// draw cloth
		if ((m_currentItem != nullptr && m_currentItem->getType() == ITEM_WEAPON) || move_type != PLAYER_SNEAK)
		{
			sf::Transform cloth_transform = m_body->getTransform();
			cloth_transform.translate(sf::Vector2f(
				m_cloth->getLocalBounds().width / 2,
				m_cloth->getLocalBounds().height / 2
				));
			states.transform = cloth_transform;
			target.draw(*m_cloth, states);
		}

		sf::Transform head_transform = m_body->getTransform();

		head_transform.translate(sf::Vector2f(
			m_body->getLocalBounds().width / 2,
			m_body->getLocalBounds().height / 2
			));
		head_transform.rotate(-m_body->getRotation());
		target.draw(*m_head, head_transform);

		// hat
		sf::Transform hat_transform = m_body->getTransform();

		hat_transform.translate(sf::Vector2f(
			m_body->getLocalBounds().width / 2,
			m_body->getLocalBounds().height / 2
			));
		hat_transform.rotate(-m_body->getRotation());
		target.draw(*m_hat, hat_transform);
	}

	/* Draw items */
	if (m_currentItem != nullptr && m_currentItem->getType() == ITEM_WEAPON)
	{
		Weapon* weapon = static_cast<Weapon*>(m_currentItem);
		if (weapon->getAutoFire())
		{
			states.transform = m_body->getTransform();
			target.draw(*m_currentItem->getWearSprite(), states);
		}
		else
		{
			states.transform = m_body->getTransform();
			target.draw(*m_currentItem->getWearSprite(), states);
		}
	}

	fatigue_bar_background->setPosition(m_body->getPosition().x, m_body->getPosition().y - 40);
	target.draw(*fatigue_bar_background);
	fatigue_bar->setPosition(m_body->getPosition().x, m_body->getPosition().y - 40);
	float scale = 1.f - static_cast<float>(fatigue) / static_cast<float>(max_fatigue);
	fatigue_bar->setScale(scale, 1);
	target.draw(*fatigue_bar);
}
sf::FloatRect Player::getScreenBounds()
{
	sf::Vector2f viewport_center = m_viewport->get().getCenter();
	sf::Vector2f viewport_size = m_viewport->get().getSize();
	return sf::FloatRect(
		viewport_center.x - viewport_size.x / 2,
		viewport_center.y - viewport_size.y / 2,
		viewport_center.x + viewport_size.x / 2,
		viewport_center.y + viewport_size.y / 2);
}

bool Player::moving()
{
	return (Input::pressed(key_left + " " + key_right + " " + key_up + " " + key_down));
}
bool Player::sneaking()
{
	return Input::pressed(key_activate_sneak);
}
bool Player::sprinting()
{
	return Input::pressed(key_activate_sprint);
}
bool Player::hasFinishedLevel()
{
	return m_levelFinished;
}
bool Player::canEmitSound()
{
	if ((getVelocity().x != 0.f || getVelocity().y != 0) && m_step_sound_clock.getElapsedTime().asMilliseconds() >= sound_emit_move_timer)
	{
		return true;
	}
	return false;
}
bool Player::isDead()
{
	return m_dead;
}

void Player::onIdle(sf::Time deltatime)
{
	if (m_currentItem == nullptr || m_currentItem->getType() != ITEM_WEAPON)
	{
		m_head->setCurrentAnimation("player_idle_head");
		m_body->setCurrentAnimation("player_idle_body");
		m_cloth->setCurrentAnimation("player_idle_cloth");
	}
	setSpeed(0);
}
void Player::onSneak(sf::Time deltatime)
{
	if (m_currentItem == nullptr || m_currentItem->getType() != ITEM_WEAPON)
	{
		step_radius = sneak_soundripple_radius;
		m_head->setCurrentAnimation("player_sneak_head");
		m_body->setCurrentAnimation("player_sneak_body");
	}
	setSpeed(sneak_speed);
}
void Player::onSprint(sf::Time deltatime)
{
	step_radius = sprint_soundripple_radius;
	bool changeAnimations = true;
	if (m_activeDisg != nullptr || (m_currentItem != nullptr && m_currentItem->getType() == ITEM_WEAPON))
	{
		changeAnimations = false;
	}
	if (changeAnimations)
	{
		m_head->setCurrentAnimation("player_sprint_head");
		m_body->setCurrentAnimation("player_sprint_body");
		m_cloth->setCurrentAnimation("player_sprint_cloth");
	}
	setSpeed(sprint_speed);
}
void Player::onWalk(sf::Time deltatime)
{
	step_radius = walk_soundripple_radius;
	if (m_currentItem == nullptr || m_currentItem->getType() != ITEM_WEAPON)
	{
		m_head->setCurrentAnimation("player_walk_head");
		m_body->setCurrentAnimation("player_walk_body");
		m_cloth->setCurrentAnimation("player_walk_cloth");
	}
	setSpeed(walk_speed);
}
void Player::whenIdle(sf::Time deltatime)
{
	if (m_lastFatigueDecreaseIdle.getElapsedTime().asMilliseconds() > fatigue_change_idle_timer)
	{
		m_lastFatigueDecreaseIdle.restart();
		changeFatigue(fatigue_change_idle);
	}
}
void Player::whenSneak(sf::Time deltatime)
{
	if (m_lastFatigueDecreaseSneak.getElapsedTime().asMilliseconds() > fatigue_change_sneak_timer)
	{
		m_lastFatigueDecreaseSneak.restart();
		changeFatigue(fatigue_change_sneak);
	}

	if (canEmitSound())
	{
		m_soundrippleManager->createRipple(m_body->getPosition(), static_cast<float>(step_radius), RIPPLE_PLAYER);
		Audiomanager::sfx(sneakSounds[thor::random(0U, sneakSounds.size() - 1)])->play();
		m_step_sound_clock.restart();
	}
}
void Player::whenWalk(sf::Time deltatime)
{
	if (m_lastFatigueDecreaseWalk.getElapsedTime().asMilliseconds() > fatigue_change_walk_timer)
	{
		m_lastFatigueDecreaseWalk.restart();
		changeFatigue(fatigue_change_walk);
	}

	if (canEmitSound())
	{
		m_soundrippleManager->createRipple(m_body->getPosition(), static_cast<float>(step_radius), RIPPLE_PLAYER);
		Audiomanager::sfx(walkSounds[thor::random(0U, walkSounds.size() - 1)])->play();
		m_step_sound_clock.restart();
	}
}
void Player::whenSprint(sf::Time deltatime)
{
	if (m_lastFatigueDecreaseSprint.getElapsedTime().asMilliseconds() > fatigue_change_sprint_timer)
	{
		m_lastFatigueDecreaseSprint.restart();
		changeFatigue(fatigue_change_sprint);
	}

	if (canEmitSound())
	{
		m_soundrippleManager->createRipple(m_body->getPosition(), static_cast<float>(step_radius), RIPPLE_PLAYER);
		Audiomanager::sfx(sprintSounds[thor::random(0U, sprintSounds.size() - 1)])->play();
		m_step_sound_clock.restart();
	}
}
void Player::slow_down()
{
	velocity.x *= 0.8;
	velocity.y *= 0.8;
	if (fabs(velocity.x) < 0.01 && fabs(velocity.y) < 0.01)
	{
		stop();
	}
}
void Player::accelerate()
{
	velocity.x *= 1.1;
	velocity.y *= 1.1;
}
void Player::setMaxSpeed(float p_maxSpeed)
{
	max_speed = p_maxSpeed;
}
void Player::setLevelFinished(bool p_value)
{
	m_levelFinished = p_value;
}
void Player::playerPositionWeapon()
{
	if (m_currentItem != nullptr)
	{
		if (m_currentItem->getType() == ITEM_WEAPON)
		{
			Weapon* weapon = static_cast<Weapon*>(m_currentItem);

			/* Gun or rifle */
			m_currentItem->getWearSprite()->setOrigin(m_currentItem->getWearSprite()->getGlobalBounds().width / 2, m_currentItem->getWearSprite()->getGlobalBounds().height / 2);
			if (weapon->getAutoFire())
			{
				if (m_head->getCurrentAnimation()->identifier != "player_rifle_head")
				{
					m_head->setCurrentAnimation("player_rifle_head");
				}
				if (m_body->getCurrentAnimation()->identifier != "player_rifle_body")
				{
					m_body->setCurrentAnimation("player_rifle_body");
				}
				if (m_cloth->getCurrentAnimation()->identifier != "player_rifle_cloth")
				{
					m_cloth->setCurrentAnimation("player_rifle_cloth");
				}
				sf::Vector2f weaponPosition;
				weaponPosition.x = 90;
				weaponPosition.y = 76;
				m_currentItem->getWearSprite()->setPosition(weaponPosition);
			}
			else
			{
				if (m_head->getCurrentAnimation()->identifier != "player_gun_head")
				{
					m_head->setCurrentAnimation("player_gun_head");
				}
				if (m_body->getCurrentAnimation()->identifier != "player_gun_body")
				{
					m_body->setCurrentAnimation("player_gun_body");
				}
				if (m_cloth->getCurrentAnimation()->identifier != "player_gun_cloth")
				{
					m_cloth->setCurrentAnimation("player_gun_cloth");
				}
				sf::Vector2f weaponPosition;
				weaponPosition.x = 124;
				weaponPosition.y = 68;
				m_currentItem->getWearSprite()->setPosition(weaponPosition);
			}
		}
	}
}
void Player::changeHP(int p_value)
{
	hp += p_value;
	if (hp > 0)
	{
		m_char->setTexture(*SpriteManager::loadImage(m_charPortraits[hp - 1]));
	}

	if (hp <= 0)
	{
		stop();
		m_dead = true;
		m_body->setLoopable(false);
		m_body->setCurrentAnimation("player_die");
		m_blood->setLoopable(false);
		m_blood->setCurrentAnimation("blood");
		
		if (m_currentItem != nullptr)
		{
			m_currentItem->setWear(false);
			m_currentItem->setCanBePickedUp(true);
			m_currentItem = nullptr;
		}

		if (m_secondaryItem != nullptr)
		{
			m_secondaryItem->setWear(false);
			m_secondaryItem->setCanBePickedUp(false);
			m_secondaryItem = nullptr;
		}
	}
	hp = Math::clamp(hp, 0, max_hp);
}
void Player::setItemManager(ItemManager* p_itemManager)
{
	m_itemManager = p_itemManager;
}

void Player::handleReleaseShoot(InputMouseEvent event)
{
	m_canShoot = true;
}
