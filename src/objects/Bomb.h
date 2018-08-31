/*
 * Enemy.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef BOMB_H_
#define BOMB_H_

#include "../ARK.h"
#include "Object.h"

class Bomb : public Object {
	public:
		static Image* s_frame1;
		static Image* s_frame2;
		static Image* s_frame3;
		
	public:
		float m_fuseTimer;
		float m_fuseDuration;
		vector<Color*> m_fuseColors;

		float m_fuseParticleTimer;
		float m_fuseParticleDuration;

		float m_introTimer;
		float m_introDuration;

		bool m_grounded;
		bool m_exploded;

	public:
		Bomb();
		virtual void reset();
		void explode();
		Color* getFuseColor();
		Image* getFrame();
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Bomb();
};

#endif /* ENEMY_H_ */
