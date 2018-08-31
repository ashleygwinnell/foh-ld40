
#ifndef ARK2D_DEFAULTGAME_INGAMESTATE_H_
#define ARK2D_DEFAULTGAME_INGAMESTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class Object;
class BlankObject;
class Player;
class Enemy;
class Pickup;
class Powerup;
class Explosion;
class Projectile;
class MyParticle;
class Bomb;
enum EnemyType : unsigned int ;
enum PowerupType : signed int ;



class InGameState : public GameState {
	public:

		Pool<Player* >* m_players;
		Player* m_player;
		Pool<Projectile* >* m_projectiles;
		Pool<Enemy* >* m_enemies;
		Pool<Bomb* >* m_bombs;
		Pool<Pickup* >* m_pickups;
		Pool<Powerup* >* m_powerups;
		Pool<MyParticle* >* m_particles;

		

		// Layers: 
		// 	Santa crash
		// 	Player die
		// 	Bomb going off.
		CameraShake* m_cameraShake;

		float m_introTimer;
		float m_introDuration;

		int m_score;

		bool m_win;
		bool m_died;
		float m_deadTimer;
		float m_deadDuration;

		float m_bombExplodedFilterTimer;
		float m_bombExplodedFilterDuration;

		float m_flashTimer;
		float m_flashDuration;

		float m_spawnSantaTimer;
		float m_spawnSantaDuration;
		float m_spawnSantaDurationMin;
		float m_spawnSantaDurationMax;

		float m_spawnMoleTimer;
		float m_spawnMoleDuration;
		int m_spawnMoleMaxConcurrent;

		float m_spawnMistletoeTimer;
		float m_spawnMistletoeDuration;
		int m_spawnMistletoeMaxConcurrent;

		float m_spawnPuddingTimer;
		float m_spawnPuddingDuration;
		int m_spawnPuddingMaxConcurrent;

		float m_spawnPowerupTimer;
		float m_spawnPowerupDuration;

		int m_santaHits;
		Image* m_currentPowerupContainer;
		Image* m_currentPowerup;
		PowerupType m_currentPowerupType;


		InGameState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		int countEnemiesOfType(EnemyType et);
		int countBoomerangs();
		void smartBomb();

		void onScoreChange();

		void spawnSanta();
		void spawnMole();
		void spawnMistletoe();
		void spawnPudding();
		void spawnPowerup();
		void spawnBlood(float x, float y, float radius, int num);
		void spawnBlood(float x, float y, float radius, int num, float minAngle, float maxAngle);
		unsigned int id();
		void reset();
		void start();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual ~InGameState();
};

#endif /* ARK2D_DEFAULTGAME_INGAMESTATE_H_ */
