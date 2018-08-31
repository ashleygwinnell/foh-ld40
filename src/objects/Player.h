/*
 * Player.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "../ARK.h"
#include "Object.h" 
class EightWayMovementComponent;
class Projectile;
class Pickup;
class Powerup;
class Bomb;
class Enemy;


class Player : public Object { 
	public: 

		static Image* s_playerGibHead;
		static Image* s_playerGibBody;
		static Image* s_playerGibHand;
		static Image* s_playerGibWeapon;

		static Image* s_playerBootsMoon;
		static Image* s_playerBootsHover;

		static Image* s_playerHalo;
		
		EightWayMovementComponent* m_movement;

		Animation* m_animationIdle;
		Image* m_boomerangHand;

		Timer* m_fireTimer;

		int m_numBoomerangs;

		float m_boomerangSpeedModifier;
		bool m_burnProtection;

		bool m_moving;
		bool m_grounded;
		bool m_facingLeft;
		float m_rotation;

		float m_introTimer;
		float m_introDuration;

		unsigned int m_boots;
		static const unsigned int BOOTS_NONE = 0;
		static const unsigned int BOOTS_MOON = 1;
		static const unsigned int BOOTS_HOVER = 2;
		static const unsigned int BOOTS_END = 2;

		unsigned int m_lives;

		int m_dieReason;
		static const unsigned int DIE_NULL = 0;
		static const unsigned int DIE_BOMB = 1;
		static const unsigned int DIE_PRESENT = 2;
		static const unsigned int DIE_MOLE = 3;
		static const unsigned int DIE_FLAMES = 4;

		Player();  
		void reset();
		void hitByProjectile(Projectile* pr);
		void hitByPickup(Pickup* pu);
		void hitByBomb(Bomb* b);
		void hitByBombExplode(Bomb* b);
		void hitByEnemy(Enemy* e);
		void die();
		void setBoots(int boots);
		void onLoseLife();
		Vector2<float> getHandPosition();
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Player();
};

#endif /* PLAYER_H_ */
