/*
 * Object.h
 * 18 April 2012
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <ARK2D/Core/Geometry/Vector2.h>
#include <ARK2D/Core/Geometry/Shape.h>
#include <ARK2D/Core/Geometry/Rectangle.h>
#include <ARK2D/Core/GameObject.h>
#include <ARK2D/Core/Tween/Easing.h>

class DefaultGame;

class Object : public GameObject {
	public:

		Shape<float>* m_bounds;
		Shape<float>* m_boundsOnCamera;
		Vector2<float>* m_velocity;
		Vector2<float>* m_velocityMax;
		Vector2<float>* m_acceleration;

		Vector2<float>* m_gravity;
		bool m_restrictVelocity;

		Object();
		void update(GameContainer* container, GameTimer* timer);
		void render(GameContainer* container, Renderer* r);

		inline Shape<float>* getBounds() { return m_bounds; }
		inline Vector2<float>* getVelocity() { return m_velocity; }

		virtual void fireParticles();

		virtual void teleport();
		virtual bool keepInBounds();
		virtual void move(float multiplier);
		virtual void gravity();
		virtual void gravity(float multiplier);
		virtual void collision(Object* other) = 0;
		virtual void onBoundsMoved();

		virtual ~Object();

	public:
		static float s_gravity;
};

#endif /* OBJECT_H_ */
