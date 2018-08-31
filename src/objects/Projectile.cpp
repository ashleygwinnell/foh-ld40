/*
 * Pickup.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Projectile.h"
#include "Player.h"
#include "../DefaultGame.h"
#include <ARK2D/Core/Tween/Timer.h>

Image* Projectile::s_spriteBoomerang = NULL;
AnimationFrameList* Projectile::s_kissAnim = NULL;
AnimationFrameList* Projectile::s_flameAnim = NULL;
Color* Projectile::s_kissColor = NULL;

Projectile::Projectile(): 
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance(); 

	if (s_spriteBoomerang == NULL) {
		s_spriteBoomerang = SpriteSheetStore::getImage("sprites/boomerang-normal.png");

		s_kissAnim = AnimationFrameList::create("sprites/kiss-*.png", 1, 4, dg->spritesheet, dg->desc);
		s_flameAnim = AnimationFrameList::create("sprites/flames/flames-all-*.png", 1, 49, dg->spritesheet, dg->desc, 2);
		s_kissColor = new Color("#d31b76");
	}
	
	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, s_spriteBoomerang->getWidth(), s_spriteBoomerang->getHeight());
	m_bounds->setLocationByCenter(0, 0);
 
	m_velocity->set(0, 0);  
	m_velocityMax->set(300, 300);    
	m_acceleration->set(0, 0); 

	m_animation = new Animation();
	m_animation->setFrameTime(0.25f);;

	reset();
 
}   
bool Projectile::isInGround() {
	return m_travelTimer >= m_travelDuration;
}
   
void Projectile::reset() {    
	m_velocity->set(0.0f, 0.0f);

	m_introTimer = 0.01f;
	m_introDuration = 0.25f;

	m_travelRotation = 0.0f;
	m_travelRotationSpeed = 180.0f;
	m_travelTimer = 0.0f;
	m_travelDuration = 2.0f;

	m_travelDurationMin = 1.0f;
	m_travelDurationMax = 4.0f;
	m_travelDistanceForMin = 50.0f;
	m_travelDistanceForMax = 500.0f;

	m_isFromPlayer = false;
	m_grounded = false;
}  
void Projectile::updatePlayerBoomerangDuration() {
	float distance = MathUtil::distance(m_from.getX(), m_from.getY(), m_to.getX(), m_to.getY());

	if (distance < m_travelDistanceForMin) {
		distance = m_travelDistanceForMin;
	} else if (distance > m_travelDistanceForMax) {
		distance = m_travelDistanceForMax;
	}
    InGameState* igs = DefaultGame::getInstance()->stateInGame;
	float diff = m_travelDurationMax - m_travelDurationMin;
	m_travelDuration = (m_travelDurationMin + Easing::easebetween(Easing::QUADRATIC_IN, distance, 0.0f, diff, m_travelDistanceForMax)) * igs->m_player->m_boomerangSpeedModifier;
}

void Projectile::start(ProjectileType type) {
	m_type = type;

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = DefaultGame::getInstance()->stateInGame;

	if (type == PT_BOOMERANG) {
		m_isFromPlayer = true;

		m_animation->clear();
		m_animation->addFrame(s_spriteBoomerang);

		m_bounds->asRectangle()->setSize(s_spriteBoomerang->getWidth(), s_spriteBoomerang->getHeight());
	} 
	else if (type == PT_KISS) {
		m_isFromPlayer = false;

		m_animation->setFrames(s_kissAnim);
		m_animation->setFrameTime(0.15f);

		float a = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_player->m_bounds->getCenterX(), igs->m_player->m_bounds->getCenterY());
		MathUtil::moveAngle<float>(m_velocity, a, MathUtil::randBetweenf(50.0f, 80.0f));

		m_bounds->asRectangle()->setSize(m_animation->getCurrentFrame()->getWidth(), m_animation->getCurrentFrame()->getHeight());
	}
	else if (type == PT_FLAME) {
		m_isFromPlayer = false;

		m_animation->setFrames(s_flameAnim);
		m_animation->setFrameTime(0.05f);
		m_animation->setCurrentFrameIndex(MathUtil::randBetween(0, 10));

		m_bounds->asRectangle()->setSize(15, 19);
	}
}
void Projectile::returnToPlayer() {
	m_to.set(m_bounds->getCenterX(), m_bounds->getCenterY());
	m_travelTimer = m_travelDuration * 0.5f;
}
void Projectile::update(GameContainer* container, GameTimer* timer) {
	//move(1.0f);

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = DefaultGame::getInstance()->stateInGame;

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	int prevFrame = m_animation->getCurrentFrameIndex();
	m_animation->update(timer);

	if (m_type == PT_BOOMERANG) { 
		if (m_travelTimer < m_travelDuration) {
			m_travelRotation += m_travelRotationSpeed * timer->getDelta();

			m_travelTimer += timer->getDelta();
			if (m_travelTimer >= m_travelDuration) {
				m_travelTimer = m_travelDuration;
			}

			float newX, newY;
			float hd = m_travelDuration * 0.5f;
			if (m_travelTimer <= hd) {
				newX = Easing::easebetween(Easing::QUARTIC_OUT, m_travelTimer, m_from.getX(), m_to.getX(), hd);
				newY = Easing::easebetween(Easing::QUADRATIC_OUT, m_travelTimer, m_from.getY(), m_to.getY(), hd);
			} else { 
				newX = Easing::easebetween(Easing::QUARTIC_IN, m_travelTimer-hd, m_to.getX(), igs->m_player->getHandPosition().getX(), hd);
				newY = Easing::easebetween(Easing::QUADRATIC_IN, m_travelTimer-hd, m_to.getY(), dg->m_floor, hd);
			}
			m_bounds->setLocationByCenter(newX, newY);
		} else {
			if (m_grounded) {
				m_travelRotation = 45;	
			}
			//m_travelRotation = 0;
		}
	}
	else if (m_type == PT_KISS) {
		Object::move(1.0f);
	}
	else if (m_type == PT_FLAME) {
		Object::move(1.0f);
		Object::gravity(1.0f);

		m_travelRotation = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY()) - 90;

		if (m_bounds->getMaxY() >= dg->m_floor) {
			m_bounds->setLocationByCenter(m_bounds->getCenterX(), dg->m_floor - (m_bounds->getHeight() * 0.5f));
			m_travelRotation = 0;
			m_velocity->set(0.0f, 0.0f);
		}

		if (prevFrame > m_animation->getCurrentFrameIndex()) {
			setPendingRemoval(true);
		}
	}



	
}

void Projectile::collision(Object* other) { 
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

} 

void Projectile::render(GameContainer* container, Renderer* r) {
	//r->setDrawColor(Color::green);
	//r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), (m_bounds->getWidth()/2.0f), 10);
	r->setDrawColor(Color::white);

	float scWidth = 1.0f;
	float scHeight = 1.0f;
	if (m_introTimer >= 0.01f) {
		scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 1.25f, 1.0f, m_introDuration);
		scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.75f, 1.0f, m_introDuration);
		if (m_type == PT_KISS) {
			scWidth = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
			scHeight = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);

		}
	}

	if (m_type == PT_KISS) {
		r->setDrawColor("#d31b76", 1.0f);
	}
	//else if (m_type == PT_FLAME) {
		//r->setDrawColor(Color::black);
	//}

	Image* sprite = m_animation->getCurrentFrame();

	float width = sprite->getWidth();
	float height = sprite->getHeight();
	
	float rot = m_travelRotation;//MathUtil::anglef(0, 0, m_velocity->getX(), m_velocity->getY());

	sprite->setWidth(width*scWidth);
	sprite->setHeight(height*scHeight);
	sprite->setRotation(rot);
	sprite->drawCentered(m_bounds->getCenterX(), m_bounds->getCenterY());
	sprite->setWidth(width);
	sprite->setHeight(height);
}

Projectile::~Projectile() { 
	 
}
