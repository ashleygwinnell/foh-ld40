/*
 * MyParticle.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef MYPARTICLE_H_
#define MYPARTICLE_H_

#include "../ARK.h"
#include "Object.h"

class MyParticleSprites {
	public:
		static AnimationFrameList* s_toastParticles;
		static AnimationFrameList* s_enemyGibParticles;
		static Image* s_energyCubeParticle;
		static Image* s_balloonGibTop;
		static Image* s_balloonGibBottom;
		static Image* s_balloonGibInsides;

		static Image* s_lineShort;
		static Image* s_lineMedium;
		static Image* s_lineLong;
		
		static Image* s_cloud11;
		static Image* s_cloud22;
		static Image* s_cloud33;
		static Image* s_cloud44;
		static Image* s_cloud55;
		static Image* s_cloud77;
		static vector<Image*>* s_bloodSelection;

		static void init();
};

class MyParticle : public Object {
	public:   

		
		static const unsigned int TYPE_TOAST = 0;
		static const unsigned int TYPE_ENEMY_FLYING_GIB = 1;
		static const unsigned int TYPE_PLAYER_GIB = 2;
		static const unsigned int TYPE_ENERGY = 3;
		static const unsigned int TYPE_BALLOON = 4;
		static const unsigned int TYPE_LEVELUP_LINE = 5;
		static const unsigned int TYPE_EXPLOSION_CIRCLE = 6;

		static const unsigned int TYPE_BLOOD = 10;
		static const unsigned int TYPE_WALKING_DUST = 11;
		static const unsigned int TYPE_PRESENT_COLLECT = 12;
		static const unsigned int TYPE_BOMB_FUSE = 17;
		static const unsigned int TYPE_BOMB_CIRCLE = 18;
		static const unsigned int TYPE_SMOKE = 19;
		static const unsigned int TYPE_MOLE_SPAWN = 20;
		
		static const unsigned int TYPE_TEXT = 21;

		Animation* m_animation;
		unsigned int m_type;

		bool m_lifetime;
		float m_lifeTimer;
		float m_lifeDuration;

		float m_lifeScaleStart;
		float m_lifeScaleEnd;

		float m_lifeAlphaStart;
		float m_lifeAlphaEnd;

		bool m_rotateToVelocity;
		float m_rotationOffset;

		bool m_useGravity;

		bool m_flipX;
		Color* m_color;
		float m_bombCircleRadius;

		string m_text;

		MyParticle(); 
		void reset();
		void start(unsigned int type);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~MyParticle();
};

#endif /* MYPARTICLE_H_ */
