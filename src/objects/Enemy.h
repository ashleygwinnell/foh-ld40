/*
 * Player.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef ENEMY_H_
#define ENEMY_H_

#include "../ARK.h"
#include "Object.h" 
class EightWayMovementComponent;
class Player;

enum EnemyType : unsigned int {
	ET_SANTA,
    ET_MOLE,
    ET_MISTLETOE,
    ET_CHRISTMASPUDDING,
    ET_GINGERBREADMAN,
    ET_DVD
};

class Enemy : public Object { 
	public: 

		static Image* s_santaSleigh;
		static AnimationFrameList* s_santaIdle;
		static AnimationFrameList* s_santaFist;
		static Image* s_santaDying;
		static Color* s_santaParticleColor;

		static AnimationFrameList* s_moleIntro;
		static Image* s_moleIdle;
		static AnimationFrameList* s_moleOutro;

		static Image* s_mistletoe1;
		static Image* s_mistletoe2;
		static Image* s_mistletoe3;
		static Image* s_mistletoe_gibLeaf;
		static Image* s_mistletoe_gibTop;
		static Image* s_pudding;

		EnemyType m_type;

		bool m_damagable;
		bool m_canDieByBomb;

		float m_introTimer;
		float m_introDuration;

		int m_hitTimes;
		float m_hitTimer;
		float m_hitDuration;
		Animation* m_animation;

		bool m_droppingBombs;
		float m_droppingBombsTimer;
		float m_droppingBombsDuration;

		bool m_blowingKisses;
		float m_blowingKissesTimer;
		float m_blowingKissesDuration;

		float m_spawnDarkParticleTimer;
		float m_spawnDarkParticleDuration;

		float m_spawnSantaParticleTimer;
		float m_spawnSantaParticleDuration;
		float m_spawnSantaParticleDurationMin;
		float m_spawnSantaParticleDurationMax;

		float m_moleTimer;
		float m_moleDuration;
		unsigned int m_moleState;
		static const unsigned int MOLE_STATE_INTRO = 0;
		static const unsigned int MOLE_STATE_IDLE = 1;
		static const unsigned int MOLE_STATE_OUTRO = 2;

		float m_puddingRotation;
		bool m_santaAlive;

		Enemy();  
		bool isDamagable();
		bool canHurtPlayer(Player* p);
		bool killableBySmartBomb();
		void reset();
		void start(EnemyType et);
		void hit();
		void die();
		void killParticles(float startAngle, float power, float powerSpread);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Enemy();
};

#endif /* ENEMY_H_ */
