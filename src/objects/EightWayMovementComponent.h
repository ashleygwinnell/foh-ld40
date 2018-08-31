/*
 * EightWayMovementComponent.h
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#ifndef EIGHTWAYMOVEMENTCOMPONENT_H_
#define EIGHTWAYMOVEMENTCOMPONENT_H_

#include "../ARK.h"
#include <ARK2D/Core/Geometry/Vector2.h>
#include <ARK2D/Core/Controls/Accelerometer.h>
#include <ARK2D/Core/Controls/Input.h>

class Object;

class EightWayMovementComponent {
	public:
		Object* m_parent;
		Vector2<float>* m_velocity;
		float m_lastAngle;

		bool m_controllerInput;
		signed int m_controllerIndex;
		unsigned int m_controllerAxisIndex;

		unsigned int m_keyUp;
		unsigned int m_keyDown;
		unsigned int m_keyLeft;
		unsigned int m_keyRight;

		unsigned int m_controllerUp;
		unsigned int m_controllerDown;
		unsigned int m_controllerLeft;
		unsigned int m_controllerRight;

		bool m_snap;

		bool m_stickPushing;
		bool m_buttonPushing;

	public:
		EightWayMovementComponent(Object* o);

		inline float getX() { return m_velocity->getX(); }
		inline float getY() { return m_velocity->getY(); }

		bool isUpDown();
		bool isDownDown();
		bool isLeftDown();
		bool isRightDown();

		bool isJumpPressed();
		
		void setController(bool b, unsigned int controllerIndex, unsigned int controllerAxisIndex);
		void setKeys(unsigned int up, unsigned int down, unsigned int left, unsigned int right);
		void setKeysArrows() { setKeys(Input::KEY_UP, Input::KEY_DOWN, Input::KEY_LEFT, Input::KEY_RIGHT); }
		void setKeysWASD() { setKeys(Input::KEY_W, Input::KEY_S, Input::KEY_A, Input::KEY_D); }
		void setSnap(bool b) { m_snap = b; }

		void reset();
		void update(GameContainer* container, GameTimer* timer);
		virtual ~EightWayMovementComponent();
};

#endif /* EIGHTWAYMOVEMENTCOMPONENT_H_ */
