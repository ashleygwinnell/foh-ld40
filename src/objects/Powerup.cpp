/*
 * Powerup.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Powerup.h"
#include "MyParticle.h"
#include "Enemy.h"
#include "Player.h"
#include "../DefaultGame.h"

#include "EightWayMovementComponent.h"
#include <ARK2D/Core/+.h>
#include <ARK2D/Audio/Sound.h>
#include <ARK2D/Util/CameraShake.h>

Image* Powerup::s_bubble = NULL;

Image* Powerup::s_iconStarOfBethlehem = NULL;
Image* Powerup::s_iconMoonBoots = NULL;
Image* Powerup::s_iconHoverboard = NULL;
Image* Powerup::s_iconFireProtection = NULL;
Image* Powerup::s_iconBoomerangs3X = NULL;
Image* Powerup::s_iconBoomerangsFast = NULL;
Image* Powerup::s_iconHalo = NULL;

Powerup::Powerup():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_bubble == NULL) {
		s_bubble = SpriteSheetStore::getImage("sprites/powerup-bubble.png");
		
		s_iconStarOfBethlehem = SpriteSheetStore::getImage("sprites/powerup-smartbomb.png");
		s_iconMoonBoots = SpriteSheetStore::getImage("sprites/powerup-moonboots.png");
		s_iconHoverboard = SpriteSheetStore::getImage("sprites/powerup-hoverboard.png");
		s_iconFireProtection = SpriteSheetStore::getImage("sprites/powerup-flame-protection.png");
		s_iconBoomerangs3X = SpriteSheetStore::getImage("sprites/powerup-multiple-boomerangs.png");
		s_iconBoomerangsFast = SpriteSheetStore::getImage("sprites/powerup-fast-boomerangs.png");
		s_iconHalo = SpriteSheetStore::getImage("sprites/powerup-extra-life.png");
	}

    m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 15.0f, 15.0f);
	m_bounds->setLocationByCenter(64, 32);

	m_velocity->set(0, 0);
	m_velocityMax->set(0, 100);
	m_acceleration->set(0, 0);

	reset();
}

void Powerup::reset() {
	m_introTimer = 0.0f;
	m_introDuration = 0.5f;

	m_offsetX = 0.0f;
	m_offsetTimer = 0.0f;
	m_offsetDuration = 1.0f;

	m_velocity->set(0.0f, 0.0f);

	m_grounded = false;
}

void Powerup::startRandom() {
	PowerupType pt = (PowerupType) MathUtil::randBetween(PT_START, PT_END+1);
	while (pt == DefaultGame::getInstance()->stateInGame->m_currentPowerupType) {
		pt = (PowerupType) MathUtil::randBetween(PT_START, PT_END+1);
	}
	start(pt);
}

void Powerup::start(PowerupType type) {

	m_type = type;
	if (type == PT_STAR_OF_BETHLEHEM) {
		m_sprite = s_iconStarOfBethlehem;
	}
	else if (type == PT_MOON_BOOTS) {
		m_sprite = s_iconMoonBoots;
	}
	else if (type == PT_HOVER_BOARD) {
		m_sprite = s_iconHoverboard;
	}
	else if (type == PT_FIRE_PROTECTION) {
		m_sprite = s_iconFireProtection;
	}
	else if (type == PT_MULTI_BOOMERANG) {
		m_sprite = s_iconBoomerangs3X;
	}
	else if (type == PT_FAST_BOOMERANG) {
		m_sprite = s_iconBoomerangsFast;
	}
	else if (type == PT_LIVES_X2) {
		m_sprite = s_iconHalo;
	}

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

}

void Powerup::update(GameContainer* container, GameTimer* timer) {
	Object::update(container, timer);

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

    DefaultGame* dg= DefaultGame::getInstance();
	if (!m_grounded) {
		Object::gravity(1.0f);
		Object::move(1.0f);

		m_offsetTimer += timer->getDelta();
		if (m_offsetTimer >= m_offsetDuration) {
			m_offsetTimer -= m_offsetDuration;
		}

		float hd = m_offsetDuration * 0.5f;
		if (m_offsetTimer < hd) {
			m_offsetX = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_offsetTimer, 0.0f, 5.0f, hd);
		} else {
			m_offsetX = Easing::easebetweenback(Easing::QUADRATIC_OUT, Easing::QUADRATIC_IN, m_offsetTimer - hd, 0.0f, -5.0f, hd);
		}
	} else {
		m_offsetX = 0.0f;
	}

}

void Powerup::collision(Object* other) {

}

void Powerup::collected(Player* player) {
	setPendingRemoval(true);

	InGameState* igs = DefaultGame::getInstance()->stateInGame;

	int boomerangsInScene = igs->countBoomerangs();
	player->m_numBoomerangs = 1;
	player->m_boomerangSpeedModifier = 1.0f;
	player->setBoots(Player::BOOTS_NONE);
	player->m_lives = 1;
	player->m_burnProtection = false;

	DefaultGame::getInstance()->m_sfxPowerup->play();
	GameContainer* container = ARK2D::getContainer();

	MyParticle* particle = igs->m_particles->get();
	particle->reset();
	particle->start(MyParticle::TYPE_TEXT);
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	particle->m_animation->addFrame(s_bubble);
	particle->m_lifetime = true;
	particle->m_lifeTimer = 0.0;
	particle->m_lifeDuration = 2.0;
	particle->m_lifeScaleStart = 1.0;
	particle->m_lifeScaleEnd = 0.5;
	particle->m_lifeAlphaStart = 1.0;
	particle->m_lifeAlphaEnd = 0.0f;
	particle->m_rotateToVelocity = false;
	particle->m_useGravity = false;

	float angle = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), container->getWidth()*0.5f, container->getHeight()*0.5f);
	MathUtil::moveAngle<float>(particle->m_velocity, angle, 60);

	igs->m_currentPowerup = m_sprite;
	igs->m_currentPowerupType = m_type;
	if (m_type == PT_STAR_OF_BETHLEHEM) {
		igs->m_currentPowerup = NULL;
		igs->m_currentPowerupType = PT_NONE;
		igs->smartBomb();
		particle->m_text = "STAR OF BETHLEHEM";
	}
	else if (m_type == PT_MOON_BOOTS) {
		player->setBoots(Player::BOOTS_MOON);
		particle->m_text = "MOON BOOTS";
	} 
	else if (m_type == PT_HOVER_BOARD) {
		player->setBoots(Player::BOOTS_HOVER);
		particle->m_text = "HOVER BOARD";
	} 
	else if (m_type == PT_FIRE_PROTECTION) {
		player->m_burnProtection = true;
		particle->m_text = "FIRE PROTECTION";
	}
	else if (m_type == PT_MULTI_BOOMERANG ) {
		player->m_numBoomerangs =  3 - boomerangsInScene;
		particle->m_text = "MULTI-BOOMERANG";

	}
	else if (m_type == PT_FAST_BOOMERANG ) {
		player->m_boomerangSpeedModifier = 0.66f;
		particle->m_text = "BOOK OF AERODYNAMICS";
	}
	else if (m_type == PT_LIVES_X2) {
		player->m_lives = 2;
		particle->m_text = "SAVING GRACE";
	}
	

	particle = igs->m_particles->get();
	particle->reset();
	particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	particle->m_animation->addFrame(s_bubble);
	particle->m_lifetime = true;
	particle->m_lifeTimer = 0.0;
	particle->m_lifeDuration = 0.5;
	particle->m_lifeScaleStart = 1.0;
	particle->m_lifeScaleEnd = 1.5;
	particle->m_lifeAlphaStart = 1.0;
	particle->m_lifeAlphaEnd = 0.0f;
	particle->m_rotateToVelocity = false;
	particle->m_useGravity = false;

	// pop effect 
	for(float angle = 0; angle < 360.0f; angle += 20) {
		float bitX = m_bounds->getCenterX();
		float bitY = m_bounds->getCenterY();
		MathUtil::moveAngle<float>(bitX, bitY, angle,s_bubble->getWidth()*0.5);
		MyParticle* particle = igs->m_particles->get();
		particle->reset();
		particle->m_bounds->setLocationByCenter(bitX, bitY);
		particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
		particle->m_lifetime = true;
		particle->m_lifeTimer = 0.0f;
		particle->m_lifeDuration = 0.5f;
		particle->m_lifeScaleStart = 1.0f;
		particle->m_lifeScaleEnd = 0.0f;
		particle->m_lifeAlphaStart = 1.0f;
		particle->m_lifeAlphaEnd = 0.0f;
		particle->m_rotateToVelocity = false;
		particle->m_useGravity = false;
		particle->m_velocity->set(0.0f, 0.0f);
		MathUtil::moveAngle<float>(particle->m_velocity, angle, 20);
	}
}

void Powerup::render(GameContainer* container, Renderer* r)
{
	InGameState* igs = DefaultGame::getInstance()->stateInGame;
	r->setDrawColor(Color::white);

	float scale = 1.0f; 
	float rotation = 0.0f; //m_rotation;

	if (m_introTimer > 0.0f) {
		scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	
	s_bubble->setAlpha(1.0f);
	m_sprite->setAlpha(1.0f);

	s_bubble->drawCenteredScaled(m_bounds->getCenterX()+m_offsetX, m_bounds->getCenterY(), scale, scale);
	m_sprite->drawCenteredScaled(m_bounds->getCenterX()+m_offsetX, m_bounds->getCenterY(), scale, scale);
}

Powerup::~Powerup() {

}
