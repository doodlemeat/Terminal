#pragma once

#include "Viewport.h"
#include "SFML\System\Clock.hpp"
#include "GObject.h"
#include "Animation.h"
#include "SFML\Graphics\Drawable.hpp"
#include <string>
#include "Input.h"

class SoundrippleManager;
class BulletManager;
class ItemManager;
class Item;
class Goal;
class Disg;

namespace sf
{
	class RectangleShape;
	class Sprite;
}

class Player : public GObject, public sf::Drawable
{
	enum PlayerMoveType
	{
		PLAYER_IDLE,
		PLAYER_SNEAK,
		PLAYER_WALK,
		PLAYER_SPRINT
	};
public:
	Player();
	~Player();

	void configure();
	void update(std::vector<Goal*> goals, sf::Time deltatime);
	void setViewport(float width, float height, float maxX, float maxY);
	void updateViewport();
	void setSoundrippleManager(SoundrippleManager* soundrippleManager);
	void setBulletManager(BulletManager* p_bulletManager);
	void changeFatigue(int p_fatique_change);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void setMoveType(int type, sf::Time deltatime, bool or = false);
	
	bool moving();
	bool sneaking();
	bool sprinting();
	bool hasFinishedLevel();
	bool canEmitSound();
	bool isDead();

	void onIdle(sf::Time deltatime);
	void onSneak(sf::Time deltatime);
	void onWalk(sf::Time deltatime);
	void onSprint(sf::Time deltatime);
	void whenIdle(sf::Time deltatime);
	void whenSneak(sf::Time deltatime);
	void whenWalk(sf::Time deltatime);
	void whenSprint(sf::Time deltatime);
	void slow_down();
	void accelerate();
	void setMaxSpeed(float p_maxSpeed);
	void setLevelFinished(bool p_value);
	void playerPositionWeapon();
	void changeHP(int p_value);
	void setItemManager(ItemManager* p_itemManager);
	void handleReleaseShoot(InputMouseEvent event);

	sf::FloatRect getScreenBounds();

	std::string key_up;
	std::string key_down;
	std::string key_left;
	std::string key_right;
	std::string key_activate_sprint;
	std::string key_activate_sneak;
	std::string key_toggle_items;
	std::string key_use;
	std::string key_shoot;
	std::string key_useaction;
	std::string key_dropitem;
	std::vector<std::string> walkSounds;
	std::vector<std::string> sprintSounds;
	std::vector<std::string> sneakSounds;

	bool m_levelFinished;
	bool m_dead;
	bool m_canDropItem;
	bool m_canShoot;
	bool m_hasThrownCoin;

	float sprint_speed;
	float walk_speed;
	float sneak_speed;
	float sprint_slowdown_factor;
	float walk_slowdown_factor;
	float sneak_slowdown_factor;
	float sound_emit_breath_radius_factor;
	float max_speed;
	float m_disguiseTimer;

	int emptyClipTimer;
	int move_type;
	int max_hp;
	int hp;
	int min_fatigue;
	int max_fatigue;
	int fatigue;
	int player_sprint_rr;
	int player_walk_rr;
	int player_sneak_rr;
	int shoot_radius;
	int step_radius;
	int level;
	int fatique_change_shoot;
	int fatigue_change_idle;
	int fatigue_change_idle_timer;
	int fatigue_change_sneak;
	int fatigue_change_sneak_timer;
	int fatigue_change_walk;
	int fatigue_change_walk_timer;
	int fatigue_change_sprint;
	int fatigue_change_sprint_timer;
	int sound_emit_move_timer;
	int sound_emit_breath_timer;
	int sneak_soundripple_radius;
	int walk_soundripple_radius;
	int sprint_soundripple_radius;
	int max_direction;
	int min_direction;

	Viewport* m_viewport;

	sf::Clock m_lastShot;
	sf::Clock m_lastFatigueDecreaseIdle;
	sf::Clock m_lastFatigueDecreaseSneak;
	sf::Clock m_lastFatigueDecreaseWalk;
	sf::Clock m_lastFatigueDecreaseSprint;
	sf::Clock m_lastSoundEmit;
	sf::Clock m_lastSoundEmitBreath;
	sf::Clock m_step_sound_clock;
	sf::Clock m_lastClipEmptySound;

	SoundrippleManager* m_soundrippleManager;
	BulletManager* m_bulletManager;
	ItemManager* m_itemManager;

	AnimatedSprite* m_hat;
	AnimatedSprite* m_head;
	AnimatedSprite* m_body;
	AnimatedSprite* m_cloth;
	AnimatedSprite* m_blood;
	
	sf::Sprite* m_char;
	sf::Sprite* m_charBackgroundHud;
	sf::Sprite* m_charFrameHud;

	sf::RectangleShape* fatigue_bar;
	sf::RectangleShape* fatigue_bar_background;

	Disg* m_activeDisg;
	Item* m_currentItem;
	Item* m_secondaryItem;

	std::map<int, std::string> m_charPortraits;
};
