/*
 * Enemy.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Enemy.h"
#include "../DefaultGame.h"

#include "Projectile.h"
#include "MyParticle.h"
#include "Pickup.h"
#include "Bomb.h"
#include "Player.h"
#include "EightWayMovementComponent.h"
//#include "../states/UpgradesState.h"

#include <ARK2D/Core/Tween/Timer.h>
#include <ARK2D/Util/CameraShake.h>

Image* Enemy::s_santaSleigh = NULL;
Image* Enemy::s_santaDying = NULL;
AnimationFrameList* Enemy::s_santaIdle = NULL;
AnimationFrameList* Enemy::s_santaFist = NULL;
Color* Enemy::s_santaParticleColor = NULL;

AnimationFrameList* Enemy::s_moleIntro = NULL;
AnimationFrameList* Enemy::s_moleOutro = NULL;
Image* Enemy::s_moleIdle = NULL;
// AnimationFrameList* Enemy::s_moleLoop = NULL;

Image* Enemy::s_mistletoe1 = NULL;
Image* Enemy::s_mistletoe2 = NULL;
Image* Enemy::s_mistletoe3 = NULL;
Image* Enemy::s_mistletoe_gibLeaf = NULL;
Image* Enemy::s_mistletoe_gibTop = NULL;
Image* Enemy::s_pudding = NULL;

Enemy::Enemy(): 
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance(); 

	if (s_santaSleigh == NULL) {
		s_santaSleigh = SpriteSheetStore::getImage("sprites/santa-sleigh.png");
		s_santaIdle = AnimationFrameList::create("sprites/santa-idle-*.png", 1, 2, dg->spritesheet, dg->desc);
		s_santaFist = AnimationFrameList::create("sprites/santa-fist-*.png", 1, 2, dg->spritesheet, dg->desc);
		s_santaParticleColor = new Color("#ffe398");
		s_santaDying = SpriteSheetStore::getImage("sprites/santa-dying.png");
		
		s_moleIntro = AnimationFrameList::create("sprites/mole-*.png", 0, 5, dg->spritesheet, dg->desc);
		s_moleOutro = AnimationFrameList::create("sprites/mole-*.png", 0, 5, dg->spritesheet, dg->desc);
		std::reverse(s_moleOutro->getFrames()->begin(),s_moleOutro->getFrames()->end());
		s_moleIdle = SpriteSheetStore::getImage("sprites/mole-5.png");
		
		s_mistletoe1 = SpriteSheetStore::getImage("sprites/mistletoe.png");
		s_mistletoe2 = SpriteSheetStore::getImage("sprites/mistletoe-2.png");
		s_mistletoe3 = SpriteSheetStore::getImage("sprites/mistletoe-3.png");
		s_mistletoe_gibLeaf = SpriteSheetStore::getImage("sprites/mistletoe-gib-leaf.png");
		s_mistletoe_gibTop = SpriteSheetStore::getImage("sprites/mistletoe-gib-top.png");

		s_pudding = SpriteSheetStore::getImage("sprites/pudding.png");
	}

	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 7, 7);
	
	m_velocity->set(0, 0);  
	m_velocityMax->set(40, 40);    
	m_acceleration->set(700, 700);

	m_animation = new Animation();
	m_animation->setFrameTime(0.5f);

	reset();
} 

bool Enemy::canHurtPlayer(Player* p) {
	if (m_type == ET_MOLE) {
		if (m_moleState == MOLE_STATE_IDLE && p->m_boots != Player::BOOTS_HOVER) {
			return true;
		}
		return false;
	}
	return true;
}
   
void Enemy::reset() {  
	DefaultGame* dg = DefaultGame::getInstance();
	GameContainer* container = ARK2D::getContainer();

	m_velocity->set(0, 0);
	m_damagable = true;
	m_canDieByBomb = true;

	m_introTimer = 0.0f;
	m_introDuration = 0.3f;

	m_hitTimes = 0;
	m_hitTimer = 0.0f;
	m_hitDuration = 0.5f;

	m_droppingBombs = false;
	m_droppingBombsTimer = 0.0f;
	m_droppingBombsDuration = 1.0f;

	m_blowingKisses = false;
	m_blowingKissesTimer = 0.0f;
	m_blowingKissesDuration = 1.5f;

	m_spawnDarkParticleTimer = 0.0f;
	m_spawnDarkParticleDuration = 0.1f;

	m_puddingRotation = 0.0f;

	m_spawnSantaParticleTimer = 0.0f;
	m_spawnSantaParticleDurationMin = 0.03f;
	m_spawnSantaParticleDurationMax = 0.06f;
	m_spawnSantaParticleDuration = MathUtil::randBetweenf(m_spawnSantaParticleDurationMin, m_spawnSantaParticleDurationMax);

	m_moleTimer = 0.0f;
	m_moleDuration = 3.0f;
	m_moleState = MOLE_STATE_INTRO;

	m_santaAlive = true;
}  

void Enemy::start(EnemyType et) {
	m_type = et;

    GameContainer* container = ARK2D::getContainer();
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	if (et == ET_SANTA) {
		m_bounds->asRectangle()->setSize(20,15);
		m_bounds->setLocationByCenter(-20, 50);
		m_canDieByBomb = false;

		m_velocity->set(100, -10.0f + MathUtil::randBetweenf(0.0f, 20.0f));

		m_animation->clear();
		m_animation->setFrameTime(0.5f);
		m_animation->setFrames(s_santaIdle);

		m_droppingBombs = true;
		m_droppingBombsDuration = 2.0f;
		m_droppingBombsTimer = MathUtil::randBetweenf(0.0f, 2.0f);
		
	}
	else if (et == ET_MOLE) {
		m_damagable = false;
		m_moleState = MOLE_STATE_INTRO;

		dg->m_sfxMoleSpawn->play();

		float x = igs->m_player->m_bounds->getCenterX() + ((MathUtil::randBetweenf(0.0f,1.0f) < 0.5f)?15:-15);

		m_bounds->asRectangle()->setSize(10,10);
		m_bounds->setLocationByCenter(x, dg->m_floor - 5);

		m_animation->clear();
		m_animation->setFrameTime(0.25f);
		m_animation->setFrames(s_moleIntro);
	}
	else if (et == ET_MISTLETOE) {
		float x = MathUtil::randBetweenf(30.0f, container->getWidth()-30);
		m_blowingKisses = true;
		m_bounds->asRectangle()->setSize(11,17);
		m_bounds->setLocationByCenter(x, -10);
		m_introTimer = 0.01f;

		m_animation->clear();
		m_animation->addFrame(s_mistletoe1);
	}
	else if (et == ET_CHRISTMASPUDDING) {
		float x = MathUtil::randBetweenf(30.0f, container->getWidth()-30);
		m_bounds->asRectangle()->setSize(9,7);
		m_bounds->setLocationByCenter(x, 0);

		m_animation->clear();
		m_animation->addFrame(s_pudding);
	}
}
void Enemy::hit() {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	
	if (m_type == ET_SANTA) {
		if (m_hitTimes == 0) {
			// crash
			Pickup* p = igs->m_pickups->get();
			p->reset();
			p->start(PU_PRESENT);
			p->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());

			m_animation->setFrameTime(0.25f);
			m_animation->setFrames(s_santaFist);

			igs->m_santaHits++;

			dg->m_sfxSantaHurt->play();
		}

		
	}
	else if (m_type == ET_MISTLETOE) {

		dg->m_sfxMistletoeHurt->play();

		if (m_hitTimes == 0) {
			m_animation->clear();
			m_animation->addFrame(s_mistletoe2);
		}
		if (m_hitTimes == 1) {
			m_animation->clear();
			m_animation->addFrame(s_mistletoe3);
		}

		MyParticle* particle;
		particle = igs->m_particles->get();
		particle->reset();
		particle->m_animation->reset();
		particle->m_animation->addFrame(s_mistletoe_gibLeaf);
		particle->m_type = MyParticle::TYPE_PLAYER_GIB;
		particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 2, m_bounds->getCenterY());
		particle->m_velocity->setX(0 - MathUtil::randBetweenf(0.0f, 10.0f));
        particle->m_rotateToVelocity = true;

		particle = igs->m_particles->get();
		particle->reset();
		particle->m_animation->reset();
		particle->m_animation->addFrame(s_mistletoe_gibLeaf);
		particle->m_type = MyParticle::TYPE_PLAYER_GIB;
		particle->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 2, m_bounds->getCenterY());
		particle->m_velocity->setX(0 + MathUtil::randBetweenf(0.0f, 10.0f));
        particle->m_rotateToVelocity = true;
		

		if (m_hitTimes >= 2) {
			

			// TODO spawn mistletoe particles. 
			die();
			//setPendingRemoval(true);
		}
	}
	else if (m_type == ET_CHRISTMASPUDDING) {
		die();
		//setPendingRemoval(true);
	}

	m_hitTimes++;
	m_hitTimer = 0.001f;
}
bool Enemy::killableBySmartBomb() {
	if (m_type == ET_SANTA) {
		return false;
	}
	return true;
}
void Enemy::die() {
	setPendingRemoval(true);
	// play sound.

	if (m_type == ET_MISTLETOE) {
        InGameState* igs = DefaultGame::getInstance()->stateInGame;
		MyParticle* particle;
		particle = igs->m_particles->get();
		particle->reset();
		particle->m_animation->reset(); 
		particle->m_animation->addFrame(s_mistletoe_gibTop);
		particle->m_type = MyParticle::TYPE_PLAYER_GIB;
		particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getMinY()+2);
        particle->m_velocity->setX(-20 + MathUtil::randBetweenf(0.0f, 40.0f));
        particle->m_velocity->setY(-100 + MathUtil::randBetweenf(0.0f, 50.0f));
        particle->m_rotateToVelocity = true;
	}

	killParticles(-90, 30, 30);
}


void Enemy::killParticles(float startAngle, float power, float powerSpread) {
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = dg->stateInGame;
    if (m_type == ET_MOLE || m_type == ET_CHRISTMASPUDDING) {
						
		DefaultGame* dg = DefaultGame::getInstance();
		InGameState* igs = dg->stateInGame;
		float diePower = 0.0f; 
		
		MyParticle* part = NULL;
		
		diePower = MathUtil::randBetween(power - powerSpread, power + powerSpread);
		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		//part->m_animation->addFrame(Enemy::s_enemyButterflyGibLeft);
        part->m_animation->addFrame(MyParticleSprites::s_cloud55);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 2, m_bounds->getCenterY());
		MathUtil::moveAngle<float>(part->m_velocity, startAngle - 45 + MathUtil::randBetweenf(0,90), diePower);

		
	}
	
	
	igs->spawnBlood(m_bounds->getCenterX(), m_bounds->getCenterY(), 1, 4);

	
	

}

bool Enemy::isDamagable() {
	return m_damagable && m_hitTimer == 0.0f;
}
void Enemy::update(GameContainer* container, GameTimer* timer) {
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = dg->stateInGame;
    
    
	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	if (m_type == ET_SANTA) {
		m_animation->update(timer);

		if (m_bounds->getMinX() > container->getWidth() + 20) {
			setPendingRemoval(true);
		}
		if (igs->m_win) {
			Object::gravity(2.0f);				
		}

		Object::move(1.0f);




		m_spawnSantaParticleTimer += timer->getDelta();
		if (!igs->m_died && !igs->m_win && m_spawnSantaParticleTimer >= m_spawnSantaParticleDuration) {
			m_spawnSantaParticleTimer -= m_spawnSantaParticleDuration;
			m_spawnSantaParticleDuration = MathUtil::randBetweenf(m_spawnSantaParticleDurationMin, m_spawnSantaParticleDurationMax);

			for(unsigned int i = 0; i < 2; i++) { 
				MyParticle* particle = igs->m_particles->get();
				particle->reset();
	            particle->m_bounds->setLocationByCenter(
	            	m_bounds->getMinX() - 7 + MathUtil::randBetweenf(0.0f, 2.0f), 
	            	m_bounds->getMaxY() + 2 + MathUtil::randBetweenf(0.0f, 3.0f)
	            );
				particle->m_type = MyParticle::TYPE_BOMB_FUSE;
				particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
				particle->m_lifetime = true;
				particle->m_lifeDuration = MathUtil::randBetweenf(0.3f, 0.6f);
	            particle->m_lifeScaleStart = 1.0f;
	            particle->m_lifeScaleEnd = MathUtil::randBetween(0.75f, 1.0f);
	            particle->m_lifeAlphaStart = 1.0f;
	            particle->m_lifeAlphaEnd = 0.0f;
	            particle->m_rotationOffset = MathUtil::randBetweenf(0.0f, 90.0f);
				particle->m_color = s_santaParticleColor;
				particle->m_velocity->set(0.0f, 0.0f);
	            particle->m_useGravity = false;
				MathUtil::moveAngle<float>(particle->m_velocity, 180, MathUtil::randBetweenf(40.0f, 60.0f));
			}

		}
	}
	else if (m_type == ET_MOLE) {
		int curFrame = m_animation->getCurrentFrameIndex();
		m_animation->update(timer);
		int nowFrame = m_animation->getCurrentFrameIndex();
		if (m_moleTimer < m_moleDuration) {
			if (nowFrame < curFrame) { // end of anim.
				m_moleState = MOLE_STATE_IDLE;
				m_animation->clear();
				m_animation->addFrame(s_moleIdle);
			}
		}

		if (m_animation->size() >= 5) {
			if (m_spawnDarkParticleTimer == 0.0f) {
				m_spawnDarkParticleTimer = 0.01f;
			}
		}

		m_moleTimer += timer->getDelta();
		if (m_moleTimer >= m_moleDuration) {
			if (m_animation->size() <= 2) {
				m_moleState = MOLE_STATE_OUTRO;
				m_animation->clear();
				m_animation->addFrames(s_moleOutro);
			} else {
				if (nowFrame < curFrame) { // end of anim.
					setPendingRemoval(true);
				}
			}
		}
		Object::move(1.0f);
	}
	else if (m_type == ET_MISTLETOE) {
		float a = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_player->m_bounds->getCenterX(),igs->m_player->m_bounds->getCenterY());
		//m_acceleration
		m_velocity->set(0.0f, 0.0f);
		MathUtil::moveAngle<float>(m_velocity, a, 10.0f);

		Object::move(1.0f);
		if (m_bounds->getCenterY() > 40) {
			m_bounds->setLocationByCenter(m_bounds->getCenterX(), 40);
		}
	}
	else if (m_type == ET_CHRISTMASPUDDING) {
		Object::gravity(1.0f);
		Object::move(1.0f);

		m_puddingRotation += 180.0f * timer->getDelta();

		if (m_bounds->getMaxY() >= dg->m_floor) {
			setPendingRemoval(true);

			dg->m_sfxPuddingExplode->play();

			Projectile* p;
			for(unsigned int i = 0; i < 10; i++) {
				p = igs->m_projectiles->get();
				p->reset();
				p->start(PT_FLAME);
				p->m_bounds->setLocationByCenter(m_bounds->getCenterX(), dg->m_floor - (p->m_bounds->getHeight()*0.5) - 1);
				MathUtil::moveAngle<float>(p->m_velocity, -110 + MathUtil::randBetweenf(0.0f, 40.0f), MathUtil::randBetweenf(50.0f, 200.0f));
			}

			
		}
	}

	if (!igs->m_died && !igs->m_win && m_blowingKisses){
		m_blowingKissesTimer += timer->getDelta();
		if (m_blowingKissesTimer >= m_blowingKissesDuration) {
			m_blowingKissesTimer -= m_blowingKissesDuration;
			Projectile* kiss = igs->m_projectiles->get();
			kiss->reset();
			kiss->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getMaxY());
			kiss->start(PT_KISS);

			dg->m_sfxMistletoeKiss->play();
		}
	}

	if (!igs->m_died && !igs->m_win && m_spawnDarkParticleTimer > 0.0f) {
		m_spawnDarkParticleTimer += timer->getDelta();
		if (m_spawnDarkParticleTimer >= m_spawnDarkParticleDuration) {
			m_spawnDarkParticleTimer = 0.0f;

			MyParticle* particle = igs->m_particles->get();
			particle->reset();
			particle->m_type = MyParticle::TYPE_MOLE_SPAWN;
			particle->m_color = Color::black.pointer();
            particle->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getMaxY());
            particle->m_animation->addFrame(MyParticleSprites::s_bloodSelection->at( MathUtil::randBetween(0, MyParticleSprites::s_bloodSelection->size()) ));
            particle->m_useGravity = false;
            particle->m_lifetime = true;
            particle->m_lifeDuration = MathUtil::randBetweenf(0.0f, 2.0f);
            MathUtil::moveAngle<float>(particle->m_velocity, MathUtil::randBetweenf(-135.0f, -45.0f), MathUtil::randBetweenf(10.0f, 30.0f));
		}
	}

	//move(1.0f);

	if (m_hitTimer > 0.0f) {
		m_hitTimer += timer->getDelta();
		if (m_hitTimer >= m_hitDuration) {
			m_hitTimer = 0.0f;
		}
	}

	if (!igs->m_died && !igs->m_win && m_droppingBombs) {
		m_droppingBombsTimer += timer->getDelta();
		if (m_droppingBombsTimer >= m_droppingBombsDuration) {
			if (m_bounds->getMinX() > 20 && m_bounds->getMaxX() < container->getWidth() - 20) {
				Bomb* b = igs->m_bombs->get();
				b->reset();
				b->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
				b->m_velocity->set(-20 + MathUtil::randBetweenf(0.0f, 40.0f), -60.0f + MathUtil::randBetween(0.0f, 30.0f));

				m_droppingBombsTimer = 0;

				dg->m_sfxBombFuse->play();
			}
		}
	}
} 
void Enemy::collision(Object* other) {  

} 

void Enemy::render(GameContainer* container, Renderer* r) 
{  
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	if (igs->m_died) {
		return;
	}	

	r->setDrawColor(Color::white);

	if (m_type == ET_SANTA) {

		s_santaSleigh->drawAligned(m_bounds->getCenterX()-2, m_bounds->getMaxY()+5, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);

		if (m_santaAlive) {
			Image* img = m_animation->getCurrentFrame();
			img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
		}
	}
	else if (m_type == ET_MOLE) {
		Image* img = m_animation->getCurrentFrame();
		img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM);
	}
	else if (m_type == ET_MISTLETOE || m_type == ET_CHRISTMASPUDDING) {
		float scale = 1.0f;
		if (m_introTimer > 0.0f) {
            scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
		}
		else if (m_hitTimer > 0.0f) {
            scale = Easing::easebetween(Easing::QUADRATIC_OUT, m_hitTimer, 1.35f, 1.0f, m_hitDuration);
		}

		Image* img = m_animation->getCurrentFrame();

		if (m_type == ET_CHRISTMASPUDDING) {
			img->setRotation(m_puddingRotation);
		}

		img->drawCenteredScaled(m_bounds->getCenterX(), m_bounds->getCenterY(), scale, scale);	
	}

	
	

	

}

Enemy::~Enemy() { 
	
}
