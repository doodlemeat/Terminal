#pragma once

#include "SFML\Graphics\Sprite.hpp"
#include "LightEngine.h"
#include "SFML\System\Time.hpp"
#include "GObject.h"
#include "Text.h"
#include "SFML\System\Clock.hpp"

class GuardState;
class Player;
class Pathfinder;
class Item;
class BulletManager;

class Guard: public GObject, public sf::Drawable
{
public:
	Guard();
	~Guard();

	void configure();
	void update(sf::Time deltatime);
	void setWayPoints(std::vector<sf::Vector2f> points);
	void createVision(LightEngine* lightEngine);
	Light* getLight();
	void drawPath();
	void setPathfinder(Pathfinder* p_pathfinder);
	void setWeapon(Item* p_item);
	void setKeycard(Item* p_item);
	void setDisguise(Item* p_item);
	void setBulletManager(BulletManager* p_bulletManager);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void changeOrigin();
	void alert();
	void setCameraDistance(float distance); // This changes every time we compute for a new camera
	float getCameraDistance();

	bool isPlayerDetected();

	void AddState(GuardState* state);
	void ChangeState();
	void SetState(std::string state);

	int currentPoint;
	int nextPoint;

	/* Used when going from chase to idle state */
	bool findBack;
	std::vector<sf::Vector2i> m_wayBack;
	int nextPointWayBack;

	/* Used in chase state */
	int guard_max_idle_second;

	bool pointSearch;
	bool initNewPoint;
	bool rotatePositive;
	float rotateTo;
	std::vector<sf::Vector2f> m_patrolPoints;

	/* Suspicious walkway */
	std::vector<sf::Vector2f> m_suspPoints;
	int nextPointSusp;
	int currentPointSusp;

	/* Chase data */
	std::vector<sf::Vector2f> m_chasePoints;
	int nextPointChase;
	int currentPointChase;

	Light* m_light;
	sf::Color m_color;
	sf::Color m_color_detected;

	float walk_speed;
	float run_speed;

	std::vector<GuardState*> states;
	GuardState* current;
	Pathfinder* m_pathfinder;
	Player* m_player;

	void changeHP(int p_value);

	bool isDead()
	{
		return dead;
	}

	int hp;

	bool dead;

	Item* weapon;
	Item* keycard;
	Item* disguise;

	BulletManager* m_bulletManager;

	sf::Clock m_lastShot;
	float m_cameraDistance;
	bool alerted;
};