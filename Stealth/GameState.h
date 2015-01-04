#pragma once

#include "State.h"
#include "SFML\Graphics\Rect.hpp"
#include "SFML\Graphics\RectangleShape.hpp"

class LightEngine;
class ObjectManager;
class Maploader;
class BulletManager;
class AnimationManager;
class Camera;
class Light;
class Player;
class Guard;
class SoundrippleManager;
class Pathfinder;
class Door;
class ItemManager;
class Goal;
class MapObject;
class Weapon;
class Button;

namespace sf
{
	class Text;
	class Sprite;
}

struct TutArea
{
	std::string identifier;
	sf::FloatRect area;
};

class GameState : public State {
public:
	GameState();
	bool Init();
	void Exit();

	bool Update();
	void Draw();

	std::string Next();
	void setNextState(std::string state);
	bool IsType(const std::string &type);

	// Game state specific
	std::vector<Light*> getLights();
	void newRound();
	void drawLoadingScreen();
	Weapon* makeWeapon(std::string identifier);
	bool loadNextLevel();
	void createWeapons();
	void createGoals();
	void createCoins();
	void createDisguises();
	void createKeycards();
	void createPlayer();
	void createCameras();
	void createGuards();

	void drawPauseScreen();
	void handleRestart();
	void handleResume();
	void handleMute();
	void handleExit();

private:
	float fps;
	float m_fadeout_factor;

	int m_currentLevel;

	bool m_exit;
	bool m_paused;
	bool m_loose;
	bool m_win;
	bool m_changeToLooseState;
	bool m_loadNextLevel;
	
	sf::Clock loadingScreen;

	std::string next_state;
	std::string m_nextLevel;

	std::vector<sf::FloatRect> m_collision_bounds;
	std::vector<Camera*> m_cameras;
	std::vector<Guard*> m_guards;
	std::vector<Door*> m_doors;
	std::vector<Goal*> m_goals;
	Player* m_player;
	sf::Text* fps_text;

	Pathfinder *m_pathfinder;
	Maploader *m_map;
	LightEngine *m_lightEngine;
	SoundrippleManager *m_soundripple_manager;
	BulletManager* m_bulletManager;
	AnimationManager* m_animationManager;
	ItemManager* m_itemManager;

	/* HUD */
	sf::Sprite* m_smallItem;
	sf::Sprite* m_bigItem;
	sf::RectangleShape m_looseFadeoutScreen;

	// Pause stuff
	sf::Sprite* m_pauseBackground;
	sf::Sprite* m_pauseTitle;
	Button* m_exitButton;
	Button* m_restartButton;
	Button* m_resumeButton;
	Button* m_muteButton;

	/* TUTORIAL HARDCODED SHIT */
	sf::Sprite* key_a;
	sf::Sprite* key_w;
	sf::Sprite* key_s;
	sf::Sprite* key_d;
	sf::Sprite* key_e;
	sf::Sprite* arrow;
	sf::Sprite* key_ctrl;
	sf::Sprite* key_shift;
	sf::Sprite* key_mouseleft;
	bool key_a_bool;
	bool key_w_bool;
	bool key_s_bool;
	bool key_d_bool;
	bool key_e_bool;
	bool draw_e_key;
	bool draw_arrow;
	bool draw_key_mouse;
	bool key_shift_bool;
	bool key_ctrl_bool;
	std::vector<TutArea> m_tutAreas;
};