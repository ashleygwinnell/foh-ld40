/*
 * Arrow.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef PICKUP_H_
#define PICKUP_H_

#include "../ARK.h"
#include "Object.h"

class Player; 
enum PickupType {
	PU_PRESENT
};

class Pickup : public Object {
	public:
		static Image* s_spritePresent;

		static Image* s_presentGibRibbon1;
		static Image* s_presentGibRibbon2;
		static Image* s_presentGibRibbon3;
		static Image* s_presentGibBox;

		PickupType m_type;
		Image* m_sprite;
		bool m_grounded;
		bool m_tumbling;

		float m_rotation;

		float m_introTimer;
		float m_introDuration;

	public:
		Pickup();
		virtual void reset();
		void collected(Player* player);
		void explode();
		virtual void start(PickupType type);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void collision(Object* other);
		virtual ~Pickup();
};

#endif /* PICKUP_H_ */
