/*
 * Bomb.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Bomb.h"
#include "Pickup.h"
#include "MyParticle.h"
#include "Enemy.h"
#include "Player.h"
#include "../DefaultGame.h"

#include "EightWayMovementComponent.h"
#include <ARK2D/Core/+.h>
#include <ARK2D/Audio/Sound.h>
#include <ARK2D/Util/CameraShake.h>

Image* Bomb::s_frame1 = NULL;
Image* Bomb::s_frame2 = NULL;
Image* Bomb::s_frame3 = NULL;

Bomb::Bomb():
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance();

	if (s_frame1 == NULL) {
		s_frame1 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/bomb-1.png"));
		s_frame1->setCenterOfRotation(s_frame1->getWidth()/2, s_frame1->getHeight());

		s_frame2 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/bomb-2.png"));
		s_frame2->setCenterOfRotation(s_frame2->getWidth()/2, s_frame2->getHeight());

		s_frame3 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/bomb-3.png"));
		s_frame3->setCenterOfRotation(s_frame3->getWidth()/2, s_frame3->getHeight());
	}

    m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 14, 14);
	m_bounds->setLocationByCenter(64, 32);

	m_velocity->set(0, 0);
	m_velocityMax->set(200, 200);
	m_acceleration->set(0, 0);

	m_fuseColors.push_back(new Color("#ffde42"));
	m_fuseColors.push_back(new Color("#e17f1f"));
	m_fuseColors.push_back(new Color("#e11f1f"));

	reset();
}

void Bomb::reset() {

	m_velocity->set(0.0f, 0.0f);

	m_fuseParticleTimer = 0.0f;
	m_fuseParticleDuration = 0.1f;

	m_fuseTimer = 0.0f;
	m_fuseDuration = 3.0f;

	m_grounded = false;
	m_exploded = false;

	m_introTimer = 0.0f;
	m_introDuration = 0.5f;
}
Color* Bomb::getFuseColor() {
	if (m_fuseTimer < m_fuseDuration * 0.33f) {
		return m_fuseColors.at(0);
	} else if (m_fuseTimer < m_fuseDuration * 0.66f) {
		return m_fuseColors.at(1);
	} 
	return m_fuseColors.at(2);
}
Image* Bomb::getFrame() {
	if (m_fuseTimer < m_fuseDuration * 0.33f) {
		return s_frame1;
	} else if (m_fuseTimer < m_fuseDuration * 0.66f) {
		return s_frame2;
	} 
	return s_frame3;
}

void Bomb::explode() {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	ARK2D::getLog()->e("Bomb explode");
	m_exploded = true;

	dg->m_sfxBomb->play();

	igs->m_flashTimer = 0.01f;
	igs->m_bombExplodedFilterTimer = 0.01f;
	
	igs->m_cameraShake->setSpeedMultiplier(1, 0.5f);
	igs->m_cameraShake->start(1, 2.0f);

	float explodeRadius = 20.0f;

	MyParticle* bombCircle = igs->m_particles->get();
	bombCircle->reset();
	bombCircle->start(MyParticle::TYPE_BOMB_CIRCLE);
	bombCircle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	bombCircle->m_bombCircleRadius = explodeRadius;

	PoolIterator<Enemy*> it_enemies = igs->m_enemies->newiteratorref();
	while(it_enemies.hasNext()) {
		Enemy* e = it_enemies.next();
		float dist = MathUtil::distance(m_bounds->getCenterX(), m_bounds->getCenterY(), 
			e->m_bounds->getCenterX(), e->m_bounds->getCenterY());
		if (dist < explodeRadius + e->m_bounds->getWidth()*0.5 && e->m_canDieByBomb) {
			e->die();
			//igs->m_processingWave = true;
            e->killParticles(0, 50, 25);
			e->fireParticles();
		}
	}
	//igs->m_processingWave = false;

	PoolIterator<Player*> it_players = igs->m_players->newiteratorref();
	while(it_players.hasNext()) {
		Player* p = it_players.next();
		float dist = MathUtil::distance(m_bounds->getCenterX(), m_bounds->getCenterY(), p->m_bounds->getCenterX(), p->m_bounds->getCenterY());
		if (dist < explodeRadius + p->m_bounds->getWidth() *0.5f) {
			p->hitByBombExplode(this);
			if (p->m_dieReason == Player::DIE_NULL) { 
				p->m_dieReason = Player::DIE_BOMB;
			}
            p->fireParticles();
		}
	}

	PoolIterator<Pickup*> it_pickups = igs->m_pickups->newiteratorref();
	while(it_pickups.hasNext()){
		Pickup* p = it_pickups.next();
		if (p->isPendingRemoval()) { continue; }

		float dist = MathUtil::distance(m_bounds->getCenterX(), m_bounds->getCenterY(), p->m_bounds->getCenterX(), p->m_bounds->getCenterY());
		if (dist < explodeRadius + p->m_bounds->getWidth() *0.5f) {
			p->explode();
		}
	}

	setPendingRemoval(true);
}

void Bomb::update(GameContainer* container, GameTimer* timer) {
	Object::update(container, timer);

	if (!m_grounded) { 
		Object::gravity(1.0f);
	} else {
		m_velocity->set(0.0f, 0.0f);
	}

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	m_fuseParticleTimer += timer->getDelta();
	if (m_fuseParticleTimer >= m_fuseParticleDuration) {
		m_fuseParticleTimer -= m_fuseParticleDuration;

		MyParticle* particle = igs->m_particles->get();
		particle->reset();
		particle->m_type = MyParticle::TYPE_BOMB_FUSE;
		particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
		particle->m_lifetime = MathUtil::randBetweenf(0.2f, 0.3f);
		particle->m_lifeDuration = MathUtil::randBetweenf(0.2f, 0.3f);
		particle->m_color = getFuseColor();
		if (m_fuseTimer < m_fuseDuration * 0.33f) {
			particle->m_bounds->setLocationByCenter(m_bounds->getCenterX()+2.5f, m_bounds->getCenterY()-6.5f);
		} else if (m_fuseTimer < m_fuseDuration * 0.66f) {
			particle->m_bounds->setLocationByCenter(m_bounds->getCenterX()+1.5f, m_bounds->getCenterY()-5.5f);
		} else {
			particle->m_bounds->setLocationByCenter(m_bounds->getCenterX()+0.5f, m_bounds->getCenterY()-4.5f);
		}
		particle->m_velocity->set(0.0f, 0.0f);
		MathUtil::moveAngle<float>(particle->m_velocity, MathUtil::randBetweenf(0.0f, 360.0f), MathUtil::randBetweenf(20.0f, 30.0f));
	}

	igs->m_cameraShake->setSpeedMultiplier(0, 0.5f);
	igs->m_cameraShake->start(0, 0.3f * (m_fuseTimer / m_fuseDuration));

	m_fuseTimer += timer->getDelta();
	if (m_fuseTimer >= m_fuseDuration) {
		explode();
	}

	Object::move(1.0f);
	// m_bounds->asRectangle()->setLocationByCenter(
	// 	m_bounds->getCenterX() + (m_velocity->getX() * timer->getDelta()),
	// 	m_bounds->getCenterY() + (m_velocity->getY() * timer->getDelta())
	// );

}
void Bomb::collision(Object* other) {

}



void Bomb::render(GameContainer* container, Renderer* r)
{
	InGameState* igs = DefaultGame::getInstance()->stateInGame;
	r->setDrawColor(Color::white);

	//Object::render(container, r);

	r->setDrawColor(Color::white);
	Image* img = getFrame();
	float scale = 1.0f; //Easing::easebetween(Easing::QUADRATIC_IN, m_fuseTimer, 1.0f, 2.0f, m_fuseDuration); 

	float rotation = 0.0f;
	// float qwd = m_fuseDuration*0.25f;
	// float maxRotation = 5.0f;
    
 //    if (m_fuseTimer > 0.0f) {
 //    	if (m_fuseTimer <= qwd) {
	// 		rotation = Easing::easebetween(Easing::LINEAR, m_fuseTimer, 0.0f, -maxRotation, qwd);
	// 	} else if (m_fuseTimer <= qwd*3) {
	// 		rotation = Easing::easebetween(Easing::LINEAR, m_fuseTimer-qwd, -maxRotation, maxRotation, qwd*2);
	// 	} else if (m_fuseTimer <= qwd*4) {
	// 		rotation = Easing::easebetween(Easing::LINEAR, m_fuseTimer-(qwd*3), maxRotation, 0.0f, qwd);
	// 	}
	//}

	if (m_introTimer > 0.0f) {
		scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}

	img->setRotation(rotation);
	img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), 0, 1, scale);
	
	//r->drawRect(m_bounds->getMinX(), m_bounds->getMinY(), m_bounds->getWidth(), m_bounds->getHeight());
}

Bomb::~Bomb() {

}
