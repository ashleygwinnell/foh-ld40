/*
 * POWERUP.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef POWERUP_H_
#define POWERUP_H_

#include "../ARK.h"
#include "Object.h"

class Player; 
enum PowerupType : signed int {
	PT_NONE = -1,
	PT_START = 0,
	
	PT_STAR_OF_BETHLEHEM = 0,
	PT_MOON_BOOTS,
	PT_HOVER_BOARD,
	PT_FIRE_PROTECTION,
	PT_MULTI_BOOMERANG,
	PT_FAST_BOOMERANG,
	PT_LIVES_X2,

	PT_END = PT_LIVES_X2
};

class Powerup : public Object {
	public:
		static Image* s_bubble;

		static Image* s_iconStarOfBethlehem;
		static Image* s_iconMoonBoots;
		static Image* s_iconHoverboard;
		static Image* s_iconFireProtection;
		static Image* s_iconBoomerangs3X;
		static Image* s_iconBoomerangsFast;
		static Image* s_iconHalo;

		PowerupType m_type;
		Image* m_sprite;

		float m_introTimer;
		float m_introDuration;

		float m_offsetX;
		float m_offsetTimer;
		float m_offsetDuration;

		bool m_grounded;

	public:
		Powerup();
		virtual void reset();
		void collected(Player* player);
		virtual void start(PowerupType type);
		virtual void startRandom();
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Powerup();
};

#endif /* POWERUP_H_ */
