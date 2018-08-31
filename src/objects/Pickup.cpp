/*
 * Pickup.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Pickup.h"
#include "MyParticle.h"
#include "Enemy.h"
#include "Player.h"
#include "../DefaultGame.h"

#include "EightWayMovementComponent.h"
#include <ARK2D/Core/+.h>
#include <ARK2D/Audio/Sound.h>
#include <ARK2D/Util/CameraShake.h>

Image* Pickup::s_spritePresent = NULL;

Image* Pickup::s_presentGibRibbon1 = NULL;
Image* Pickup::s_presentGibRibbon2 = NULL;
Image* Pickup::s_presentGibRibbon3 = NULL;
Image* Pickup::s_presentGibBox = NULL;

Pickup::Pickup():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_spritePresent == NULL) {
		s_spritePresent = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/present-1.png"));

		s_presentGibRibbon1 = SpriteSheetStore::getImage("sprites/present-gib-1.png");
		s_presentGibRibbon2 = SpriteSheetStore::getImage("sprites/present-gib-2.png");
		s_presentGibRibbon3 = SpriteSheetStore::getImage("sprites/present-gib-4.png");
		s_presentGibBox = SpriteSheetStore::getImage("sprites/present-gib-3.png");
	}

    m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 7.0f, 9.0f);
	m_bounds->setLocationByCenter(64, 32);

	m_velocity->set(0, 0);
	m_velocityMax->set(100, 100);
	m_acceleration->set(0, 0);

	reset();
}

void Pickup::reset() {
	m_grounded = false;
	m_tumbling = false;
	m_rotation = 0.0f;

	m_introTimer = 0.0f;
	m_introDuration = 0.5f;
}

void Pickup::start(PickupType type) {
	if (type == PU_PRESENT) {
		m_tumbling = true;
		m_sprite = s_spritePresent;

		m_velocity->set(0.0f, 0.0f);
		MathUtil::moveAngle<float>(m_velocity, MathUtil::randBetweenf(-120, -60), MathUtil::randBetweenf(50.0f, 80.0f));
	}
	m_introTimer = 0.01f;
	
}

void Pickup::update(GameContainer* container, GameTimer* timer) {
	Object::update(container, timer);

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	if (!m_grounded) { 
		Object::gravity(1.0f);
		if (m_tumbling) {
			m_rotation += 360.0f * timer->getDelta();
		}
	} else {
		m_rotation = 0.0f;
		m_velocity->set(0.0f, 0.0f);
	}

	Object::move(1.0f);

	//DefaultGame* dg = DefaultGame::getInstance();
	//InGameState* igs = dg->stateInGame;

	// m_bounds->asRectangle()->setLocationByCenter(
	// 	m_bounds->getCenterX() + (m_velocity->getX() * timer->getDelta()),
	// 	m_bounds->getCenterY() + (m_velocity->getY() * timer->getDelta())
	// );

}

void Pickup::collision(Object* other) {

}
void Pickup::collected(Player* player) {
	
	
	InGameState* igs = DefaultGame::getInstance()->stateInGame;
	igs->m_score += 1;

	igs->onScoreChange();

	DefaultGame::getInstance()->m_sfxPresentPickup->play();

	explode();
}
void Pickup::explode() {
    InGameState* igs = DefaultGame::getInstance()->stateInGame;
	setPendingRemoval(true);
	// MyParticle* particle = igs->m_particles->get();
	// particle->reset();
	// particle->start(MyParticle::TYPE_PRESENT_COLLECT);
	// particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	// particle->m_animation->addFrame(m_sprite);

	MyParticle* particle;
	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibRibbon1);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibRibbon2);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY() - 4);
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibRibbon3);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	// box bits
	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 2, m_bounds->getCenterY() - 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibBox);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 2, m_bounds->getCenterY() - 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibBox);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 2, m_bounds->getCenterY() + 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibBox);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_type = MyParticle::TYPE_PLAYER_GIB;
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 2, m_bounds->getCenterY() + 2);
	particle->m_animation->reset();
	particle->m_animation->addFrame(s_presentGibBox);
	MathUtil::moveAngle<float>(particle->m_velocity, -180 + MathUtil::randBetweenf(0,180), MathUtil::randBetweenf(50, 100));
}

void Pickup::render(GameContainer* container, Renderer* r)
{
	InGameState* igs = DefaultGame::getInstance()->stateInGame;
	r->setDrawColor(Color::white);

	float scale = 1.0f; 
	float rotation = m_rotation;

	if (m_introTimer > 0.0f) {
		scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	
	m_sprite->setAlpha(1.0f);
	if (!m_grounded) { 
		m_sprite->setRotation(rotation);
		m_sprite->drawCenteredScaled(float(m_bounds->getCenterX()), float(ceil(m_bounds->getCenterY())), scale, scale);
	} else {
		m_sprite->setRotation(rotation);
		m_sprite->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale);
	}
	
	//r->drawRect(m_bounds->getMinX(), m_bounds->getMinY(), m_bounds->getWidth(), m_bounds->getHeight());
}

Pickup::~Pickup() {

}
