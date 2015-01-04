#include "State.h"
#include "SFML\System\String.hpp"
#include "SFML\Graphics.hpp"
#include "Text.h"
#include "Window.h"
#include "GameState.h"
#include "SpriteManager.h"
#include "Audiomanager.h"
#include "Input.h"
#include "Viewport.h"
#include "Config.h"
#include "Maploader.h"
#include "LightEngine.h"
#include "Angle.h"
#include "Animation.h"
#include "BulletManager.h"
#include "Camera.h"
#include "Font.h"
#include "Player.h"
#include "Guard.h"
#include "SoundrippleManager.h"
#include "Door.h"
#include "Pathfinder.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "ItemManager.h"
#include "WeaponManager.h"
#include "Button.h"

#include "Weapon.h"
#include "Coin.h"
#include "Keycard.h"
#include "Disg.h"

#include "SFML/Audio/Music.hpp"
#include "Goal.h"

GameState::GameState()
{
	m_bulletManager         = nullptr;
	m_lightEngine           = nullptr;
	m_soundripple_manager   = nullptr;
	m_itemManager           = nullptr;
	m_map                   = nullptr;
	m_pathfinder            = nullptr;
	m_player                = nullptr;
	m_pauseBackground = nullptr;
	m_exitButton = nullptr;
	m_restartButton = nullptr;
	m_resumeButton = nullptr;
	m_muteButton = nullptr;
	m_pauseTitle = nullptr;
	key_a = nullptr;
	key_w = nullptr;
	key_s = nullptr;
	key_d = nullptr;
	key_ctrl = nullptr;
	key_shift = nullptr;
	key_e = nullptr;
	arrow = nullptr;
	key_mouseleft = nullptr;

	draw_key_mouse = false;
	key_a_bool = false;
	key_w_bool = false;
	key_s_bool = false;
	key_d_bool = false;
	key_e_bool = false;
	key_shift_bool = false;
	key_ctrl_bool = false;
	draw_e_key = true;
	draw_arrow = true;
	m_nextLevel = "";
	m_currentLevel = 1;
}
bool GameState::Init()
{
	m_paused = false;
	m_exit = false;
	m_fadeout_factor = Config::root_node["general"]["loose_fadeout_factor"].as<float>();
	m_loose = false;
	m_win = false;
	m_changeToLooseState = false;
	m_looseFadeoutScreen.setFillColor(sf::Color(0, 0, 0, 0));
	m_looseFadeoutScreen.setSize(sf::Vector2f(Window::get("main")->getSize().x, Window::get("main")->getSize().y));
	m_pathfinder = new Pathfinder();
	m_bulletManager = new BulletManager();
	m_lightEngine = new LightEngine();
	m_soundripple_manager = new SoundrippleManager();
	m_itemManager = new ItemManager();

	m_map = new Maploader("../maps/map_" + boost::lexical_cast<std::string>(m_currentLevel) +".tmx");
	m_map->Load(m_pathfinder);
	m_map->retrieveSegments(m_lightEngine, "light");

	Audiomanager::stopAllMFX();
	Audiomanager::getPlaylist("Game_Playlist")->play();
	Audiomanager::sfx("breath")->play(-1);

	// Set collision bounds
	m_collision_bounds = m_map->getBoundsByGroup("collision");
	m_soundripple_manager->setCollisionBounds(m_map->getBoundsByGroup("collision"));

	// Animation manager
	m_animationManager = new AnimationManager();

	/* -------- HUD --------*/
	createPlayer();
	createWeapons();
	createGoals();
	createCoins();
	createDisguises();
	createKeycards();
	createCameras();
	createGuards();
	
	if (m_goals.size() == 0)
	{
		std::cout << "No goals were found" << std::endl;
		return false;
	}

	if (m_itemManager->getItemsByType(ITEM_KEYCARD).size() == 0)
	{
		std::cout << "No keycards was found" << std::endl;
		return false;
	}
	
	// Create pause stuff
	m_pauseBackground = new sf::Sprite();
	m_pauseBackground->setTexture(*SpriteManager::loadImage("gui/optionsbg.png"));
	m_pauseBackground->setOrigin(sf::Vector2f(m_pauseBackground->getGlobalBounds().width / 2, m_pauseBackground->getGlobalBounds().height / 2));
	m_pauseBackground->setPosition(sf::Vector2f(Window::get("main")->getSize().x / 2, Window::get("main")->getSize().y / 2));
	std::cout << "Created pause background" << std::endl;
	
	float buttonYSpace = 20;
	float buttonUOffset = 70;
	float currentY = m_pauseBackground->getGlobalBounds().top + buttonUOffset;

	m_pauseTitle = new sf::Sprite();
	m_pauseTitle->setTexture(*SpriteManager::loadImage("hud/paused_title.png"));
	m_pauseTitle->setPosition(sf::Vector2f(Window::get("main")->getSize().x / 2 - m_pauseTitle->getGlobalBounds().width / 2, currentY));
	currentY += m_pauseTitle->getGlobalBounds().height + buttonUOffset;

	m_exitButton = new Button();
	m_exitButton->setSize(100, 40);
	m_exitButton->getSprite()->setTexture(*SpriteManager::loadImage("hud/exit_button.png"));
	m_exitButton->getSprite()->setPosition(sf::Vector2f(Window::get("main")->getSize().x / 2 - m_exitButton->getSprite()->getGlobalBounds().width / 2, currentY));
	m_exitButton->addCallback(DOWN, std::bind(&GameState::handleExit, this));
	currentY += m_exitButton->getHeight() + buttonYSpace;

	m_restartButton = new Button();
	m_restartButton->setSize(100, 40);
	m_restartButton->getSprite()->setTexture(*SpriteManager::loadImage("hud/restart_button.png"));
	m_restartButton->getSprite()->setPosition(sf::Vector2f(Window::get("main")->getSize().x / 2 - m_restartButton->getSprite()->getGlobalBounds().width / 2, currentY));
	m_restartButton->addCallback(DOWN, std::bind(&GameState::handleRestart, this));
	currentY += m_restartButton->getHeight() + buttonYSpace;

	m_muteButton = new Button();
	m_muteButton->setSize(155, 44);
	m_muteButton->getSprite()->setTexture(*SpriteManager::loadImage("hud/mute_button.png"));
	m_muteButton->getSprite()->setPosition(sf::Vector2f(Window::get("main")->getSize().x / 2 - m_muteButton->getSprite()->getGlobalBounds().width / 2, currentY));
	m_muteButton->setToggleable(true);
	m_muteButton->addCallback(DOWN, std::bind(&GameState::handleMute, this));

	m_resumeButton = new Button();
	m_resumeButton->setSize(100, 40);
	m_resumeButton->getSprite()->setTexture(*SpriteManager::loadImage("hud/resume_button.png"));
	m_resumeButton->getSprite()->setPosition(sf::Vector2f(Window::get("main")->getSize().x / 2 - m_resumeButton->getSprite()->getGlobalBounds().width / 2, currentY));
	m_resumeButton->addCallback(DOWN, std::bind(&GameState::handleResume, this));
	currentY += m_resumeButton->getHeight() + buttonYSpace;


	// tutorial init
	key_w = new sf::Sprite();
	key_w->setTexture(*SpriteManager::loadImage("W.png"));
	key_w->setOrigin(32, 32);
	
	key_a = new sf::Sprite();
	key_a->setTexture(*SpriteManager::loadImage("A.png"));
	key_a->setOrigin(32, 32);

	key_s = new sf::Sprite();
	key_s->setTexture(*SpriteManager::loadImage("S.png"));
	key_s->setOrigin(32, 32);

	key_d = new sf::Sprite();
	key_d->setTexture(*SpriteManager::loadImage("D.png"));
	key_d->setOrigin(32, 32);

	key_ctrl = new sf::Sprite();
	key_ctrl->setTexture(*SpriteManager::loadImage("ctrl.png"));
	key_ctrl->setOrigin(48, 32);

	key_shift = new sf::Sprite();
	key_shift->setTexture(*SpriteManager::loadImage("shift.png"));
	key_shift->setOrigin(48, 32);

	key_e = new sf::Sprite();
	key_e->setTexture(*SpriteManager::loadImage("E.png"));
	key_e->setOrigin(32, 32);

	arrow = new sf::Sprite();
	arrow->setTexture(*SpriteManager::loadImage("arrow.png"));
	arrow->setScale(0.180, 0.180);
	arrow->setOrigin(32, 32);

	key_mouseleft = new sf::Sprite();
	key_mouseleft->setTexture(*SpriteManager::loadImage("mouse_left.png"));
	key_mouseleft->setScale(0.5, 0.5);
	key_mouseleft->setOrigin(23.5, 52);

	std::vector<MapObject*> tutAreas = m_map->getGroup("tutorial")->getObjectsByType("tutorial");
	for (auto &tut_area : tutAreas)
	{
		TutArea tutArea;
		tutArea.identifier = tut_area->name;
		tutArea.area = sf::FloatRect(tut_area->getPosition().x, tut_area->getPosition().y, tut_area->width, tut_area->height);
		m_tutAreas.push_back(tutArea);
		std::cout << "Created tutArea with name: " << tutArea.identifier << std::endl;
	}

	return true;
}
void GameState::Exit()
{
	Text::cleanup();

	for (unsigned int i = 0; i < m_goals.size(); i++)
	{
		delete m_goals[i];
	}
	m_goals.clear();

	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		delete m_cameras[i];
	}
	m_cameras.clear();

	for (unsigned int i = 0; i < m_guards.size(); i++)
	{
		delete m_guards[i];
	}
	m_guards.clear();

	/* ------- HUD deletion -----*/

	delete fps_text;
	fps_text = nullptr;

	delete m_player;
	m_player = nullptr;

	delete m_soundripple_manager;
	m_soundripple_manager = nullptr;

	delete m_bulletManager;
	m_bulletManager = nullptr;

	delete m_itemManager;
	m_itemManager = nullptr;

	delete m_lightEngine;
	m_lightEngine = nullptr;

	delete m_pathfinder;
	m_pathfinder = nullptr;
	
	delete m_map;
	m_map = nullptr;

	// Delete pause stuff
	delete m_pauseBackground;
	delete m_exitButton;
	delete m_restartButton;
	delete m_resumeButton;
	delete m_muteButton;
	delete m_pauseTitle;
	delete key_a;
	delete key_w;
	delete key_s;
	delete key_d;
	delete key_ctrl;
	delete key_shift;
	delete key_e;
	delete arrow;
	m_pauseBackground = nullptr;
	m_exitButton = nullptr;
	m_restartButton = nullptr; 
	m_resumeButton = nullptr;
	m_muteButton = nullptr;
	m_pauseTitle = nullptr;
	key_a = nullptr;
	key_w = nullptr;
	key_s = nullptr;
	key_d = nullptr;
	key_e = nullptr;
	arrow = nullptr;
	key_ctrl = nullptr;
	key_shift = nullptr;

	delete m_animationManager;
}
bool GameState::Update()
{
	Audiomanager::getPlaylist("Game_Playlist")->update();
	if (Input::pressed_once("escape p"))
	{
		m_paused = !m_paused;
	}

	if (m_paused)
	{
		sf::View view = Window::get("main")->getDefaultView();
		Window::get("main")->setView(view);
		m_restartButton->update(sf::Mouse::getPosition());
		m_resumeButton->update(sf::Mouse::getPosition());
		m_exitButton->update(sf::Mouse::getPosition());
		m_muteButton->update(sf::Mouse::getPosition());
		Window::get("main")->setView(m_player->m_viewport->get());
	}
	if (m_exit)
	{
		setNextState("MenuState");
		return false;
	}

	if (m_paused) return true;


	if (m_player->isDead())
	{
		m_loose = true;
		if (m_changeToLooseState)
		{
			setNextState("GameOverState");
			return false;
		}
	}

	/* Check if we can load the next level */
	if (m_loadNextLevel)
	{
		// load next level
		if (!loadNextLevel())
		{
			setNextState("GameOverState");
			return false;
		}
		return true;
	}

	/* Check if the player has won the level */
	if (m_player->hasFinishedLevel())
	{
		newRound();
		return true;
	}

	if (m_win)
	{
		setNextState("WinGameState");
		return false;
	}

	m_bulletManager->update(m_player, m_guards, m_collision_bounds, Window::get("main")->deltatime);

	// Set viewport to the players viewport and update it
	m_player->update(m_goals, Window::get("main")->deltatime);
	m_player->updateViewport();
	Window::get("main")->setView(*m_player->m_viewport->getView());


	// update tutorial stuff
	sf::Vector2f playerPosition = m_player->m_body->getPosition();
	key_w->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y - 196));
	key_s->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y - 128));
	key_a->setPosition(sf::Vector2f(playerPosition.x - 64, playerPosition.y - 128));
	key_d->setPosition(sf::Vector2f(playerPosition.x + 64, playerPosition.y - 128));
	key_ctrl->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y - 128));
	key_shift->setPosition(sf::Vector2f(playerPosition.x, playerPosition.y - 128));

	/*
		Hämtar ut PLUPP ur item vektorn.
		It can stand itself in front of the other Coins because of needeness.
		DEt kan vi ställa oss.
	*/
	std::vector<Item*> PLUPP = m_itemManager->getItemsByType(ITEM_KEYCARD);
	arrow->setPosition(PLUPP[0]->getGroundSprite()->getPosition() + sf::Vector2f(-32, -64));
	if (!PLUPP[0]->isWearing())
	{
		// Make sure so that thi arrow is drawn when needed to. 
		draw_arrow = true;
	}
	else
	{
		// Här ritar vi inte ut pilen. eftersom det kan man ställs sig
		draw_arrow = false;
	}

	if (!key_w_bool && Input::pressed("w"))
	{
		key_w_bool = true;
		Audiomanager::sfx("success")->play();
	}
	if (!key_a_bool && Input::pressed("a"))
	{
		key_a_bool = true;
		Audiomanager::sfx("success")->play();
	}
	if (!key_s_bool && Input::pressed("s"))
	{
		key_s_bool = true;
		Audiomanager::sfx("success")->play();
	}
	if (!key_d_bool && Input::pressed("d"))
	{
		key_d_bool = true;
		Audiomanager::sfx("success")->play();
	}
	if (!key_ctrl_bool && Input::pressed("left_ctrl"))
	{
		key_ctrl_bool = true;
		Audiomanager::sfx("success")->play();
	}
	if (!key_shift_bool && Input::pressed("left_shift"))
	{
		key_shift_bool = true;
		Audiomanager::sfx("success")->play();
	}



	// Let player collide with bounds(walls and like everything static)
	m_player->collideWith(m_collision_bounds);

	// Update all cameras
	for (auto &camera: m_cameras)
	{
		camera->update(Window::get("main")->deltatime);
		
		if (m_player->m_activeDisg == nullptr && camera->getLight()->detectPlayer(m_player->m_body->getPosition()))
		{
			if (camera->isActive()) continue;
			// Now we need to call the nearest guard if that guard has weapon and ammo
			std::vector<Guard*> avaibleGuards;
			for (auto &guard : m_guards)
			{
				float dx = guard->hitbox.getPosition().x - camera->getPosition().x;
				float dy = guard->hitbox.getPosition().y - camera->getPosition().y;
				float distance = sqrtf(dx * dx + dy * dy);
				if (distance <= camera->getAlertDistance())
				{
					if (guard->weapon != nullptr && static_cast<Weapon*>(guard->weapon)->getAmmo() > 0)
					{
						guard->setCameraDistance(distance);
						avaibleGuards.push_back(guard);
					}
				}
			}

			// Find the closest guard
			if (avaibleGuards.size() > 0)
			{
				if (avaibleGuards.size() == 1)
				{
					avaibleGuards[0]->alert();
				}
				else
				{
					int lowestIndex = 0;
					for (unsigned int i = 0; i < avaibleGuards.size(); i++)
					{
						if (avaibleGuards[i]->getCameraDistance() < avaibleGuards[lowestIndex]->getCameraDistance())
						{
							lowestIndex = i;
						}
					}
					avaibleGuards[lowestIndex]->alert();
				}
			}
			else
			{
				for (auto &guard : m_guards)
				{
					guard->alert();
				}
			}

			if (Audiomanager::mfx("alarm")->music->getStatus() == sf::Music::Stopped)
				Audiomanager::mfx("alarm")->play();
			camera->setActive(true);
		}
		else
		{
			camera->setActive(false);
		}
	}

	// Update all guards
	for (unsigned int i = 0; i < m_guards.size(); i++)
	{
		m_guards[i]->update(Window::get("main")->deltatime);
	}

	// Update all sound ripples
	m_soundripple_manager->update(Window::get("main")->deltatime);

	// Update all lights
	m_lightEngine->update(getLights());
	
	/* Check if we stand on a item and pick it up */
	for (auto &item: m_itemManager->getItems())
	{
		if (item->isWearing())
			continue;

		if (item->getType() == ITEM_COIN)
		{
			Coin* coin = static_cast<Coin*>(item);
			coin->update(m_collision_bounds, Window::get("main")->deltatime);
		}
		sf::FloatRect a = m_player->m_body->getGlobalBounds();
		sf::FloatRect b = item->getGroundSprite()->getGlobalBounds();
		if (!(a.left > b.left + b.width ||
			a.left + a.width < b.left ||
			a.top > b.top + b.height ||
			a.top + a.height < b.top))
		{
			if (item->getType() == ITEM_COIN)
			{
				key_e->setPosition(item->getGroundSprite()->getPosition() + sf::Vector2f(48, -48));
			}
			item->setCanBePickedUp(true);
		}
		else
		{
			item->setCanBePickedUp(false);
		}

		if (item->canBePickedUp() && Input::pressed_once(m_player->key_use))
		{
			if (item->getType() == ITEM_COIN)
			{
				draw_key_mouse = true;
				if (!key_e_bool)
				{
					draw_e_key = false;
					Audiomanager::sfx("success")->play();
					key_e_bool = true;
				}
			}
			/* Pickup item */
			if (m_player->m_currentItem == nullptr)
			{
				m_player->m_currentItem = item;
			}
			else if (m_player->m_secondaryItem == nullptr)
			{
				m_player->m_secondaryItem = m_player->m_currentItem;
				m_player->m_currentItem = item;
			} 
			else if (m_player->m_currentItem != nullptr && m_player->m_secondaryItem != nullptr)
			{
				m_player->m_currentItem->getGroundSprite()->setPosition(m_player->m_body->getPosition());
				m_player->m_currentItem->setWear(false);
				m_player->m_currentItem->setCanBePickedUp(true);
				m_player->m_currentItem = item;
				m_player->m_currentItem->setWear(true);
				m_player->m_currentItem->setCanBePickedUp(true);
			}
			item->setWear(true);
			item->setCanBePickedUp(false);
			m_player->playerPositionWeapon();
			m_player->setMoveType(m_player->move_type, Window::get("main")->deltatime, true);
		}
	}

	if (m_player->m_currentItem != nullptr && m_player->m_currentItem->getType() == ITEM_COIN)
	{
		key_mouseleft->setPosition(m_player->m_body->getPosition() + sf::Vector2f(0, -128));
	}

	if (m_player->m_hasThrownCoin)
	{
		draw_key_mouse = false;
	}

	return true;
}
void GameState::Draw()
{
	m_map->Draw(*Window::get("main"), MAP_DRAW_NORMAL);
	Window::get("main")->draw(*m_lightEngine);

	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		Window::get("main")->draw(*m_cameras[i]);
	}

	/* Draw guard */
	for (auto &guard: m_guards)
	{
		Window::get("main")->draw(*guard);
	}


	/* Draw soundripples */
	Window::get("main")->draw(*m_soundripple_manager);

	for (auto &door : m_doors)
	{
		Window::get("main")->draw(*door->getSprite());
	}
	Window::get("main")->draw(*m_bulletManager);
	Window::get("main")->draw(*m_itemManager);

	/* Draw player */
	Window::get("main")->draw(*m_player);

	m_map->Draw(*Window::get("main"), MAP_DRAW_BLOCKING);

	// draw tutorial stuff
	for (auto &tutArea : m_tutAreas)
	{
		if (tutArea.area.contains(m_player->m_body->getPosition()))
		{
			if (tutArea.identifier == "wasd")
			{
				if (!key_w_bool) Window::get("main")->draw(*key_w);
				if (!key_a_bool) Window::get("main")->draw(*key_a);
				if (!key_s_bool) Window::get("main")->draw(*key_s);
				if (!key_d_bool) Window::get("main")->draw(*key_d);
			}
			if (tutArea.identifier == "sprint")
			{
				if (!key_shift_bool) Window::get("main")->draw(*key_shift);
			}
			if (tutArea.identifier == "sneak")
			{
				if (!key_ctrl_bool) Window::get("main")->draw(*key_ctrl);
			}
		}
	}
	if (draw_e_key)
	{
		for (auto &item : m_itemManager->getItemsByType(ITEM_COIN))
		{
			key_e->setPosition(item->getGroundSprite()->getPosition() + sf::Vector2f(48, -48));
			Window::get("main")->draw(*key_e);
		}
	}
	if (draw_arrow)
	{
		Window::get("main")->draw(*arrow);
	}
	if (draw_key_mouse)
	{
		Window::get("main")->draw(*key_mouseleft);
	}

	Window::get("main")->setView(Window::get("main")->getDefaultView());
	
	/* draw HUD */
	if (m_player->m_secondaryItem != nullptr)
		Window::get("main")->draw(*m_player->m_secondaryItem->getSecondarySprite());

	Window::get("main")->draw(*m_player->m_charBackgroundHud);
	Window::get("main")->draw(*m_player->m_char);
	Window::get("main")->draw(*m_player->m_charFrameHud);

	if (m_player->m_currentItem != nullptr)
		Window::get("main")->draw(*m_player->m_currentItem->getPrimarySprite());

	if (m_loadNextLevel)
	{
		drawLoadingScreen();
	}

	if (m_paused)
	{
		drawPauseScreen();
	}

	Window::get("main")->setView(m_player->m_viewport->get());

	if (m_player->isDead())
	{
		m_looseFadeoutScreen.setOrigin(Window::get("main")->getSize().x / 2, Window::get("main")->getSize().y / 2);
		m_looseFadeoutScreen.setPosition(m_player->m_viewport->getView()->getCenter());
		int new_alpha = (m_looseFadeoutScreen.getFillColor().a + 1) * m_fadeout_factor;
		m_looseFadeoutScreen.setFillColor(sf::Color(0, 0, 0, new_alpha));
		if (new_alpha >= 255)
		{
			m_looseFadeoutScreen.setFillColor(sf::Color(0, 0, 0, 255));
			m_changeToLooseState = true;
		}
		Window::get("main")->draw(m_looseFadeoutScreen);
	}
}
std::string GameState::Next()
{
	return next_state;
}
void GameState::setNextState(std::string state)
{
	next_state = state;
}
bool GameState::IsType(const std::string &type)
{
	return (type == "GameState");
}

// Game state specific
std::vector<Light*> GameState::getLights()
{
	std::vector<Light*> lights;
	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		lights.push_back(m_cameras[i]->getLight());
	}

	for (unsigned int i = 0; i < m_guards.size(); i++)
	{
		lights.push_back(m_guards[i]->getLight());
	}
	return lights;
}
void GameState::newRound()
{
	std::cout << "Process finished level" << std::endl;
	m_currentLevel++;
	m_player->setLevelFinished(false);

	/* If there is a level after this, load it */
	if (m_map->hasProperty("next_level"))
	{
		m_nextLevel = m_map->getProperty("next_level");
		m_loadNextLevel = true;
	}
	else
	{
		std::cout << "There were no more levels. You won the game" << std::endl;
		m_win = true;
	}
}
void GameState::drawLoadingScreen()
{
	sf::Text loading_next_level;
	loading_next_level.setFont(*Font::get("tahoma"));
	loading_next_level.setString("Loading level " + std::to_string(m_currentLevel));
	Window::get("main")->draw(loading_next_level);
}
Weapon* GameState::makeWeapon(std::string identifier)
{
	WeaponSkeleton* skeleton = m_itemManager->getWeaponManager()->getSkeleton(identifier);
	if (skeleton != nullptr)
	{
		WeaponSkeleton* skeleton = m_itemManager->getWeaponManager()->getSkeleton(identifier);
		Weapon* weapon = new Weapon();
		weapon->setType(ITEM_WEAPON);
		weapon->setName(identifier);
		weapon->setAmmo(skeleton->ammo);
		weapon->setAutoFire(skeleton->auto_fire);
		weapon->setBulletSpread(skeleton->bullet_spread);
		weapon->setDamage(skeleton->damage);
		weapon->setEquipped(false);
		weapon->setFireRate(skeleton->fire_rate);
		weapon->setWeight(skeleton->weight);
		weapon->getGroundSprite()->setTexture(*SpriteManager::loadImage(m_itemManager->getWeaponManager()->getSpritePath() + skeleton->groundImage));
		weapon->getWearSprite()->setTexture(*SpriteManager::loadImage(m_itemManager->getWeaponManager()->getSpritePath() + skeleton->wearImage));
		weapon->setPrimaryIconSprite(skeleton->primaryHudIcon);
		weapon->getPrimarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["big_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["big_item_background"]["offset_y"].as<float>());
		weapon->getSecondarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["small_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["small_item_background"]["offset_y"].as<float>());
		weapon->setSecondaryIconSprite(skeleton->secondaryHudIcon);
		weapon->setGroundSpriteSize(skeleton->ground_image_size);
		return weapon;
	}
	return nullptr;
}
bool GameState::loadNextLevel()
{
	std::cout << "Resetting game state..." << std::endl;
	std::cout << "Stopping music.." << std::endl;
	Audiomanager::stopAllMFX();

	std::cout << "Deallocating goals..." << std::endl;
	for (unsigned int i = 0; i < m_goals.size(); i++)
	{
		delete m_goals[i];
	}
	m_goals.clear();

	std::cout << "Deallocating cameras..." << std::endl;
	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		delete m_cameras[i];
	}
	m_cameras.clear();

	std::cout << "Deallocating guards..." << std::endl;
	for (unsigned int i = 0; i < m_guards.size(); i++)
	{
		delete m_guards[i];
	}
	m_guards.clear();

	std::cout << "Deallocating player..." << std::endl;
	delete m_player;
	m_player = nullptr;

	delete m_soundripple_manager;
	m_soundripple_manager = nullptr;
	std::cout << "Deallocating soundripple manager..." << std::endl;

	delete m_bulletManager;
	m_bulletManager = nullptr;
	std::cout << "Deallocating bullet manager..." << std::endl;

	delete m_itemManager;
	m_itemManager = nullptr;
	std::cout << "Deallocating item manager..." << std::endl;

	delete m_lightEngine;
	m_lightEngine = nullptr;
	std::cout << "Deallocating lightengine..." << std::endl;

	delete m_pathfinder;
	m_pathfinder = nullptr;
	std::cout << "Deallocating pathfinder..." << std::endl;
	
	delete m_map;
	m_map = nullptr;
	std::cout << "Deallocating map..." << std::endl;

	// Creation of new thing
	m_pathfinder = new Pathfinder();
	m_bulletManager = new BulletManager();
	m_lightEngine = new LightEngine();
	m_soundripple_manager = new SoundrippleManager();
	m_itemManager = new ItemManager();

	std::string new_map = "../maps/map_" + boost::lexical_cast<std::string>(m_currentLevel)+".tmx";
	std::cout << "Loading new map " << new_map << std::endl;
	m_map = new Maploader(new_map);
	m_map->Load(m_pathfinder);
	m_map->retrieveSegments(m_lightEngine, "light");
	m_collision_bounds = m_map->getBoundsByGroup("collision");
	m_soundripple_manager->setCollisionBounds(m_map->getBoundsByGroup("collision"));

	createPlayer(); std::cout << "Creating player..." << std::endl;
	createWeapons(); std::cout << "Creating weapons..." << std::endl;
	createGoals(); std::cout << "Creating goals..." << std::endl;
	createCoins(); std::cout << "Creating coins..." << std::endl;
	createDisguises(); std::cout << "Creating disguises..." << std::endl;
	createKeycards(); std::cout << "Creating keycards..." << std::endl;
	createCameras(); std::cout << "Creating cameras..." << std::endl;
	createGuards(); std::cout << "Creating guards..." << std::endl;

	if (m_goals.size() == 0)
	{
		std::cout << "No goals were found" << std::endl;
		return false;
	}

	if (m_itemManager->getItemsByType(ITEM_KEYCARD).size() == 0)
	{
		std::cout << "No keycards was found" << std::endl;
		return false;
	}

	std::cout << "Continue playing music..." << std::endl;

	m_loadNextLevel = false;
	return true;
}
void GameState::createWeapons()
{
	std::vector<MapObject*> weapons = m_map->getGroup("items")->getObjectsByType("weapon");
	std::cout << weapons.size() << " weapons found!" << std::endl;
	for (unsigned int i = 0; i < weapons.size(); i++)
	{
		if (weapons[i]->hasProperty("name"))
		{
			Weapon* wpn = makeWeapon(weapons[i]->getProperty("name"));
			if (wpn != nullptr)
			{
				wpn->getGroundSprite()->setPosition(weapons[i]->getPosition());
				wpn->setWear(false);
				wpn->setCanBePickedUp(true);
				m_itemManager->addItem(wpn);
			}
		}
	}
}
void GameState::createGoals()
{
	std::vector<MapObject*> goals = m_map->getGroup("entities")->getObjectsByType("goal");
	for (auto &obj : goals)
	{
		Goal* goal = new Goal();
		goal->useKeycard(true);
		goal->setArea(obj->getPosition().x, obj->getPosition().y, obj->width, obj->height);
		m_goals.push_back(goal);
	}
}
void GameState::createCoins()
{
	std::vector<MapObject*> coins = m_map->getGroup("items")->getObjectsByType("coin");
	std::string coin_sprite_path = Config::root_node["items"]["coin"]["ground_image"].as<std::string>();
	sf::Vector2f ground_image_size;
	ground_image_size.x = Config::root_node["items"]["coin"]["ground_image_size"][0].as<int>();
	ground_image_size.y = Config::root_node["items"]["coin"]["ground_image_size"][1].as<int>();
	std::string coin_sprite_path_primary = Config::root_node["items"]["coin"]["hud_primary"].as<std::string>();
	std::string coin_sprite_path_secondary = Config::root_node["items"]["coin"]["hud_secondary"].as<std::string>();
	for (unsigned int i = 0; i < coins.size(); i++)
	{
		Coin* coin = new Coin();
		coin->setType(ITEM_COIN);
		coin->getGroundSprite()->setTexture(*SpriteManager::loadImage(m_itemManager->getSpritePath() + coin_sprite_path));
		coin->getGroundSprite()->setPosition(coins[i]->getPosition());
		coin->setGroundSpriteSize(ground_image_size);
		coin->getGroundSprite()->setOrigin(ground_image_size.x / 2.f, ground_image_size.y / 2.f);
		coin->setPrimaryIconSprite(coin_sprite_path_primary);
		coin->getPrimarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["big_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["big_item_background"]["offset_y"].as<float>());
		coin->getSecondarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["small_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["small_item_background"]["offset_y"].as<float>());
		coin->setSecondaryIconSprite(coin_sprite_path_secondary);
		coin->setSoundRippleManager(m_soundripple_manager);
		coin->setSoundRippleRadius(m_itemManager->getSoundRippleRadius());
		m_itemManager->addItem(coin);
	}
}
void GameState::createDisguises()
{
	std::vector<MapObject*> disgs = m_map->getGroup("items")->getObjectsByType("disg");
	std::string disg_sprite_path = Config::root_node["items"]["disg"]["ground_image"].as<std::string>();
	sf::Vector2f ground_image_size;
	ground_image_size.x = Config::root_node["items"]["disg"]["ground_image_size"][0].as<int>();
	ground_image_size.y = Config::root_node["items"]["disg"]["ground_image_size"][1].as<int>();
	std::string disg_sprite_path_primary = Config::root_node["items"]["disg"]["hud_primary"].as<std::string>();
	std::string disg_sprite_path_secondary = Config::root_node["items"]["disg"]["hud_secondary"].as<std::string>();
	for (unsigned int i = 0; i < disgs.size(); i++)
	{
		Disg* disg = new Disg();
		disg->setType(ITEM_DISG);
		disg->getGroundSprite()->setPosition(disgs[i]->getPosition());
		disg->getGroundSprite()->setTexture(*SpriteManager::loadImage(m_itemManager->getSpritePath() + disg_sprite_path));
		disg->setGroundSpriteSize(ground_image_size);
		disg->getGroundSprite()->setOrigin(ground_image_size.x / 2.f, ground_image_size.y / 2.f);
		disg->setPrimaryIconSprite(disg_sprite_path_primary);
		disg->getPrimarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["big_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["big_item_background"]["offset_y"].as<float>());
		disg->getSecondarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["small_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["small_item_background"]["offset_y"].as<float>());
		disg->setSecondaryIconSprite(disg_sprite_path_secondary);
		m_itemManager->addItem(disg);
	}
}
void GameState::createKeycards()
{
	std::vector<MapObject*> keycards = m_map->getGroup("items")->getObjectsByType("keycard");
	std::string keycards_sprite_path = Config::root_node["items"]["keycard"]["ground_image"].as<std::string>();
	sf::Vector2f ground_image_size;
	ground_image_size.x = Config::root_node["items"]["keycard"]["ground_image_size"][0].as<int>();
	ground_image_size.y = Config::root_node["items"]["keycard"]["ground_image_size"][1].as<int>();
	std::string keycard_sprite_path_primary = Config::root_node["items"]["keycard"]["hud_primary"].as<std::string>();
	std::string keycard_sprite_path_secondary = Config::root_node["items"]["keycard"]["hud_secondary"].as<std::string>();
	for (unsigned int i = 0; i < keycards.size(); i++)
	{
		Keycard* keycard = new Keycard();
		keycard->setType(ITEM_KEYCARD);
		keycard->getGroundSprite()->setPosition(keycards[i]->getPosition());
		keycard->getGroundSprite()->setTexture(*SpriteManager::loadImage(m_itemManager->getSpritePath() + keycards_sprite_path));
		keycard->setGroundSpriteSize(ground_image_size);
		keycard->getGroundSprite()->setOrigin(ground_image_size.x / 2.f, ground_image_size.y / 2.f);
		keycard->setPrimaryIconSprite(keycard_sprite_path_primary);
		keycard->getPrimarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["big_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["big_item_background"]["offset_y"].as<float>());
		keycard->getSecondarySprite()->setPosition(m_player->m_charFrameHud->getPosition().x + Config::root_node["hud"]["small_item_background"]["offset_x"].as<float>(), m_player->m_charFrameHud->getPosition().y + Config::root_node["hud"]["small_item_background"]["offset_y"].as<float>());
		keycard->setSecondaryIconSprite(keycard_sprite_path_secondary);
		m_itemManager->addItem(keycard);
	}
}
void GameState::createPlayer()
{
	m_player = new Player();
	m_player->configure();

	// blood
	m_player->m_blood->addAnimation(m_animationManager->get("blood"));
	m_player->m_blood->addAnimation(m_animationManager->get("empty"));
	m_player->m_blood->setCurrentAnimation("empty");
	m_player->m_blood->setOrigin(
		m_player->m_blood->getLocalBounds().width / 2,
		m_player->m_blood->getLocalBounds().height / 2
		);

	m_player->m_hat->addAnimation(m_animationManager->get("player_hat_disg"));
	m_player->m_hat->addAnimation(m_animationManager->get("empty"));
	m_player->m_hat->setCurrentAnimation("empty");
	m_player->m_hat->setOrigin(
		m_player->m_hat->getLocalBounds().width / 2,
		m_player->m_hat->getLocalBounds().height / 2
		);

	// player head
	m_player->m_head->addAnimation(m_animationManager->get("player_idle_head"));
	m_player->m_head->addAnimation(m_animationManager->get("player_walk_head"));
	m_player->m_head->addAnimation(m_animationManager->get("player_sprint_head"));
	m_player->m_head->addAnimation(m_animationManager->get("player_sneak_head"));
	m_player->m_head->addAnimation(m_animationManager->get("player_gun_head"));
	m_player->m_head->addAnimation(m_animationManager->get("player_rifle_head"));
	m_player->m_head->setCurrentAnimation("player_idle_head");
	m_player->m_head->setOrigin(
		m_player->m_head->getLocalBounds().width / 2,
		m_player->m_head->getLocalBounds().height / 2
		);

	// player body
	m_player->m_body->addAnimation(m_animationManager->get("player_idle_body"));
	m_player->m_body->addAnimation(m_animationManager->get("player_walk_body"));
	m_player->m_body->addAnimation(m_animationManager->get("player_sprint_body"));
	m_player->m_body->addAnimation(m_animationManager->get("player_sneak_body"));
	m_player->m_body->addAnimation(m_animationManager->get("player_gun_body"));
	m_player->m_body->addAnimation(m_animationManager->get("player_rifle_body"));
	m_player->m_body->addAnimation(m_animationManager->get("player_die"));
	m_player->m_body->addAnimation(m_animationManager->get("player_body_disg"));
	m_player->m_body->setCurrentAnimation("player_idle_body");
	m_player->m_body->setOrigin(
		m_player->m_body->getLocalBounds().width / 2,
		m_player->m_body->getLocalBounds().height / 2
		);

	// player cloth
	m_player->m_cloth->addAnimation(m_animationManager->get("player_idle_cloth"));
	m_player->m_cloth->addAnimation(m_animationManager->get("player_walk_cloth"));
	m_player->m_cloth->addAnimation(m_animationManager->get("player_sprint_cloth"));
	m_player->m_cloth->addAnimation(m_animationManager->get("player_gun_cloth"));
	m_player->m_cloth->addAnimation(m_animationManager->get("player_rifle_cloth"));
	m_player->m_cloth->addAnimation(m_animationManager->get("player_cloth_disg"));
	m_player->m_cloth->setCurrentAnimation("player_idle_cloth");
	m_player->m_cloth->setOrigin(
		m_player->m_cloth->getLocalBounds().width / 2,
		m_player->m_cloth->getLocalBounds().height / 2
		);

	// player viewport
	m_player->setViewport(static_cast<float>(Window::get("main")->width),
		static_cast<float>(Window::get("main")->height),
		static_cast<float>(m_map->m_width * m_map->m_tileWidth),
		static_cast<float>(m_map->m_height * m_map->m_tileHeight)
		);

	m_player->m_body->setPosition(m_map->getGroup("entities")->getObjectsByName("player_spawn")[0]->getPosition());
	m_player->m_body->setOrigin(sf::Vector2f(
		m_player->m_body->getLocalBounds().width / 2.f,
		m_player->m_body->getLocalBounds().height / 2.f)
		);

	m_player->setHitBox("player");
	m_player->hitbox.setPosition(sf::Vector2f(m_player->m_body->getPosition().x + m_player->hitbox_start.x, m_player->m_body->getPosition().y + m_player->hitbox_start.y));
	m_player->setSoundrippleManager(m_soundripple_manager);
	m_player->setBulletManager(m_bulletManager);
	m_player->setItemManager(m_itemManager);
	m_player->m_viewport->getView()->zoom(0.88f);
}
void GameState::createCameras()
{
	std::vector<MapObject*> cameras = m_map->getGroup("entities")->getObjectsByType("camera");
	for (unsigned int i = 0; i < cameras.size(); i++)
	{
		Camera* camera = new Camera();
		camera->getSprite()->setTexture(*SpriteManager::loadImage("camera.png"));
		camera->getSprite()->setPosition(cameras[i]->getPosition());
		camera->getSprite()->setOrigin(
			camera->getSprite()->getLocalBounds().width / 2.f,
			camera->getSprite()->getLocalBounds().height / 2.f);
		camera->createVision(m_lightEngine);
		camera->setRotation(cameras[i]->propertyFloat("rotation"), cameras[i]->propertyFloat("rotation_value"));
		camera->setRotationSpeed(cameras[i]->propertyFloat("rotation_speed"));
		if (cameras[i]->hasProperty("rotation_direction"))
		{
			camera->setRotationDirection(cameras[i]->propertyBool("rotation_direction"));
		}
		camera->setPlayer(m_player);
		m_cameras.push_back(camera);
	}
}
void GameState::createGuards()
{
	std::vector<MapObject*> walkways = m_map->getGroup("entities")->getObjectsByType("walkway");
	std::vector<MapObject*> guards = m_map->getGroup("entities")->getObjectsByType("guard");
	for (unsigned int i = 0; i < guards.size(); i++)
	{
		Guard* guard = new Guard();
		guard->configure();
		guard->getAnimatedSprite()->addAnimation(m_animationManager->get("guard1_walk"));
		guard->getAnimatedSprite()->addAnimation(m_animationManager->get("guard1_run"));
		guard->getAnimatedSprite()->addAnimation(m_animationManager->get("guard1_die1"));
		guard->getAnimatedSprite()->addAnimation(m_animationManager->get("guard_shoot"));
		guard->getAnimatedSprite()->setCurrentAnimation("guard1_walk");
		guard->getAnimatedSprite()->setPosition(guards[i]->getPosition());
		guard->getAnimatedSprite()->setOrigin(
			guard->getAnimatedSprite()->getGlobalBounds().width / 2,
			guard->getAnimatedSprite()->getGlobalBounds().height / 2);
		guard->createVision(m_lightEngine);

		/* Does the guard has a walkway */
		if (guards[i]->hasProperty("walkway"))
		{
			/* Find the walkway by name */
			for (auto walkway : walkways)
			{
				if (walkway->name == guards[i]->getProperty("walkway"))
				{
					std::cout << "Found walkway for guard" << std::endl;
					MapObjectPolyline* ww = static_cast<MapObjectPolyline*>(walkway);
					guard->setWayPoints(ww->points);
					break;
				}
			}
		}

		/* Does the guard has a weapon? */
		if (guards[i]->hasProperty("weapon"))
		{
			Weapon* weapon = makeWeapon(guards[i]->getProperty("weapon"));
			if (weapon != nullptr)
			{
				weapon->setWear(true);
				weapon->setEquipped(true);
				m_itemManager->addItem(weapon);
				guard->setWeapon(weapon);
			}
		}

		guard->setHitBox("guards");
		guard->m_player = m_player;
		guard->setPathfinder(m_pathfinder);
		guard->setBulletManager(m_bulletManager);
		m_guards.push_back(guard);
	}
}
void GameState::drawPauseScreen()
{
	Window::get("main")->draw(*m_pauseBackground);
	Window::get("main")->draw(*m_pauseTitle);
	Window::get("main")->draw(*m_exitButton);
	Window::get("main")->draw(*m_restartButton);
	Window::get("main")->draw(*m_muteButton);
	Window::get("main")->draw(*m_resumeButton);
}
void GameState::handleRestart()
{
	loadNextLevel();
	m_paused = false;
}
void GameState::handleExit()
{
	m_exit = true;
	m_paused = false;
}
void GameState::handleResume()
{
	m_paused = false;
}
void GameState::handleMute()
{
	if (!Audiomanager::isMuted()) 
		Audiomanager::muteAll(); 
	else 
		Audiomanager::unmuteAll(100);
}