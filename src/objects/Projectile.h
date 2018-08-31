/*
 * BlankObject.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "../ARK.h"
#include "Object.h"

enum ProjectileType {
	PT_BOOMERANG,
	PT_KISS,
	PT_FLAME
};

class Projectile : public Object {
	public:   

		static Image* s_spriteBoomerang;
		static AnimationFrameList* s_kissAnim;
		static AnimationFrameList* s_flameAnim;
		static Color* s_kissColor;
		
		Animation* m_animation;

		float m_introTimer; // squash & stretch them? 
		float m_introDuration;

		ProjectileType m_type;

		Vector2<float> m_from;
		Vector2<float> m_to;

		float m_travelRotation;
		float m_travelRotationSpeed;
		float m_travelTimer;
		float m_travelDuration;
		float m_travelDurationMin;
		float m_travelDurationMax;
		float m_travelDistanceForMin;
		float m_travelDistanceForMax;
		bool m_isFromPlayer;
		bool m_grounded;

		Projectile(); 
		void reset();
		void returnToPlayer();
		bool isInGround();
		void updatePlayerBoomerangDuration();
		void start(ProjectileType type);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Projectile();
};

#endif /* PICKUP_H_ */
