
#include "InGameState.h"
#include "../objects/Player.h"
#include "../objects/Projectile.h"
#include "../objects/Enemy.h"
#include "../objects/Pickup.h"
#include "../objects/Powerup.h"
#include "../objects/MyParticle.h"
#include "../objects/Bomb.h"
#include "../DefaultGame.h"

#include <ARK2D/Core/+.h>
#include <ARK2D/UI/+.h>
#include <ARK2D/Util/CameraShake.h>

InGameState::InGameState():
	GameState()
	{

}

unsigned int InGameState::id() {
	return StatesList::STATE_INGAME;
}

void InGameState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {

}

void InGameState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void InGameState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();
	
	m_players = new Pool<Player*>(4);
	m_player = m_players->get();
	m_projectiles = new Pool<Projectile*>(5);
	m_enemies = new Pool<Enemy*>(20);
	m_bombs = new Pool<Bomb*>(5);
	m_pickups = new Pool<Pickup*>(10);
	m_powerups = new Pool<Powerup*>(10);
	m_particles = new Pool<MyParticle*>(30);

	m_cameraShake = new CameraShake(game, 3);

	m_currentPowerupContainer = SpriteSheetStore::getImage("sprites/ui-corner-box.png");

	start();
}

void InGameState::reset() {
	m_player->reset();
	m_projectiles->reset();
	m_enemies->reset();
	m_bombs->reset();
	m_pickups->reset();
	m_powerups->reset();
	m_particles->reset();

	m_score = 0;

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_deadTimer = 0.0f;
	m_deadDuration = 2.0f;

	m_bombExplodedFilterTimer = 0.0f;
	m_bombExplodedFilterDuration = 1.0f;

	m_flashTimer = 0.0f;
	m_flashDuration = 0.25f;

	m_spawnSantaDurationMin = 3.0f;
	m_spawnSantaDurationMax = 5.0f;
	m_spawnSantaDuration = MathUtil::randBetweenf(m_spawnSantaDurationMin, m_spawnSantaDurationMax);
	m_spawnSantaTimer = MathUtil::randBetweenf(0.0f, m_spawnSantaDuration);

	m_spawnMoleDuration = 6.0f;
	m_spawnMoleTimer = MathUtil::randBetweenf(0.0f, m_spawnMoleDuration);
	m_spawnMoleMaxConcurrent = 0;

	m_spawnMistletoeDuration = 7.0f;
	m_spawnMistletoeTimer = MathUtil::randBetweenf(0.0f, m_spawnMistletoeDuration);
	m_spawnMistletoeMaxConcurrent = 0;

	m_spawnPuddingDuration = 5.0f; 
	m_spawnPuddingTimer = 0.0f;
	m_spawnPuddingMaxConcurrent = 0;

	m_spawnPowerupDuration = 10.0f;
	m_spawnPowerupTimer = 0.0f; //MathUtil::randBetweenf(m_spawnPowerupDuration*0.5f, m_spawnPowerupDuration);
	
	m_santaHits = 0;

	m_currentPowerup = NULL;
	m_currentPowerupType = PT_NONE;
}

void InGameState::onScoreChange() {
	
	if (m_score >= 0) {
		m_spawnMoleMaxConcurrent = 0;
		m_spawnMistletoeMaxConcurrent = 0;
		m_spawnPuddingMaxConcurrent = 0;
	}

	if (m_score >= 1) {
		m_spawnMoleMaxConcurrent = 1;
		m_spawnMistletoeMaxConcurrent = 1;
	}
	if (m_score >= 2) {
		m_spawnPuddingMaxConcurrent = 1;
	}

	if (m_score >= 3) {
		m_spawnPuddingMaxConcurrent = 1;
	}

	if (m_score >= 5) {
		m_spawnMoleMaxConcurrent = 2;
		m_spawnMistletoeMaxConcurrent = 2;
	}

	if (m_score >= 8) {
		m_spawnMoleMaxConcurrent = 2;
		m_spawnMistletoeMaxConcurrent = 3;
		m_spawnPuddingMaxConcurrent = 3;
	}

	if (m_score >= 10) {
		m_spawnMoleMaxConcurrent = 4;
		m_spawnMistletoeMaxConcurrent = 4;
		m_spawnPuddingMaxConcurrent = 4;
	}

	if (m_score >= 12) {
        DefaultGame* dg = DefaultGame::getInstance();
		smartBomb();

		int santas = countEnemiesOfType(ET_SANTA);
		if (santas > 0) {
			PoolIterator<Enemy*> it_enemies = m_enemies->newiteratorref();
			while(it_enemies.hasNext()){
				Enemy* e = it_enemies.next();
				if (e->m_type == ET_SANTA) {
					e->m_santaAlive = false;

					MyParticle* particle = m_particles->get();
					particle->reset();
					particle->m_type = MyParticle::TYPE_PLAYER_GIB;
					particle->m_bounds->setLocationByCenter(e->m_bounds->getCenterX(), e->m_bounds->getCenterY());
					particle->m_animation->reset();
					particle->m_animation->addFrame(Enemy::s_santaDying);
					particle->m_useGravity = true;
					particle->m_rotateToVelocity = true;
					particle->m_lifeScaleStart = 1.0f;
					particle->m_lifeScaleEnd = 0.0f;
					particle->m_lifetime = true;
					MathUtil::moveAngle<float>(particle->m_velocity, 90, MathUtil::randBetweenf(0, 1.0));
					dg->m_sfxBomb->play();

				}
			}
		}

		PoolIterator<Powerup*> it_powerups = m_powerups->newiteratorref();
		while(it_powerups.hasNext()){
			it_powerups.next()->setPendingRemoval(true);
		}
		PoolIterator<Bomb*> it_bombs = m_bombs->newiteratorref();
		while(it_bombs.hasNext()){
			it_bombs.next()->setPendingRemoval(true);
		}
		PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
		while(it_projectiles.hasNext()){
			it_projectiles.next()->setPendingRemoval(true);
		}
		m_flashTimer = 0.01f;

		
		dg->m_musicCurrent->stop();
		//dg->enterState(dg->stateSummary);
		m_deadTimer = 0.01f;
		m_deadDuration = 4.0f;
		m_win = true;
	}
}

void InGameState::start() {
	reset();
	m_died = false;
	m_win = false;
}

int InGameState::countBoomerangs(){
	int count = 0;
	PoolIterator<Projectile*> it = m_projectiles->newiteratorref();
	while(it.hasNext()) {
		Projectile* e = it.next();
		if (!e->isPendingRemoval() && e->m_type == PT_BOOMERANG) {
			count++;
		}
	}
	return count;
}
int InGameState::countEnemiesOfType(EnemyType et) {
	int count = 0;
	PoolIterator<Enemy*> it = m_enemies->newiteratorref();
	while(it.hasNext()) {
		Enemy* e = it.next();
		if (!e->isPendingRemoval() && e->m_type == et) {
			count++;
		}
	}
	return count;
}

void InGameState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	DefaultGame* dg = DefaultGame::getInstance();
	Input* in = ARK2D::getInput();

	if (!dg->m_musicCurrent->isPlaying() && m_deadTimer == 0.0f) { 
		dg->m_musicCurrent->play(); 
	}

	if (in->isKeyPressed(Input::KEY_ESCAPE) ||
		in->isKeyPressed(Input::KEY_ENTER) ||
		in->isKeyPressed(Input::KEY_BACKSPACE) ||
		in->isGamepadButtonPressed(Gamepad::BUTTON_START)) {
		m_player->die();
	}

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}

	/*if (in->isKeyPressed(Input::KEY_Y)) {
		spawnSanta();
	}
	else if (in->isKeyPressed(Input::KEY_U)) {
		spawnMole();
	}
	else if (in->isKeyPressed(Input::KEY_T)) {
		spawnMistletoe();
	}
	else if (in->isKeyPressed(Input::KEY_H)) {
		spawnPudding();
	}
	else if (in->isKeyPressed(Input::KEY_P)) {
		spawnPowerup();
	}
	else if (in->isKeyPressed(Input::KEY_0)) {
		smartBomb();
	}
	else if (in->isKeyPressed(Input::KEY_J)) {
		int nextBoots = m_player->m_boots + 1;
		if (nextBoots > Player:: BOOTS_END) {
			nextBoots = 0;
		}
		m_player->setBoots(nextBoots);
	}*/
	// if (in->isKeyPressed(Input::KEY_P)) {
	// 	m_score = 11;
	// 	onScoreChange();
	// }

	if (m_flashTimer > 0.0f) {
		m_flashTimer += timer->getDelta();
		if (m_flashTimer >= m_flashDuration) {
			m_flashTimer = 0.0f;
		}
	}

	m_players->updateAll(container, timer);
	m_projectiles->updateAll(container, timer);
	m_enemies->updateAll(container, timer);
	m_bombs->updateAll(container, timer);
	m_pickups->updateAll(container, timer);
	m_powerups->updateAll(container, timer);
	m_particles->updateAll(container, timer);

	m_cameraShake->update(container, timer);

	if (!m_died && !m_win) {
		if (countEnemiesOfType(ET_SANTA) == 0) {
			m_spawnSantaTimer += timer->getDelta();
			if (m_spawnSantaTimer >= m_spawnSantaDuration) {	
				spawnSanta();
				m_spawnSantaTimer = 0.0f;
				m_spawnSantaDuration = MathUtil::randBetweenf(m_spawnSantaDurationMin, m_spawnSantaDurationMax);
			}
		}

		if (countEnemiesOfType(ET_MOLE) < m_spawnMoleMaxConcurrent) {
			m_spawnMoleTimer += timer->getDelta();
			if (m_spawnMoleTimer >= m_spawnMoleDuration) {
				spawnMole();
				m_spawnMoleTimer = 0.0f;
			}
		}

		if (countEnemiesOfType(ET_MISTLETOE) < m_spawnMistletoeMaxConcurrent) {
			m_spawnMistletoeTimer += timer->getDelta();
			if (m_spawnMistletoeTimer >= m_spawnMistletoeDuration) {
				spawnMistletoe();
				m_spawnMistletoeTimer = 0.0f;
			}
		}

		if (countEnemiesOfType(ET_CHRISTMASPUDDING) < m_spawnPuddingMaxConcurrent) {
			m_spawnPuddingTimer += timer->getDelta();
			if (m_spawnPuddingTimer >= m_spawnPuddingDuration) {
				spawnPudding();
				m_spawnPuddingTimer = 0.0f;
			}
		}

		m_spawnPowerupTimer += timer->getDelta();
		if (m_spawnPowerupTimer >= m_spawnPowerupDuration && m_powerups->sizeActive() == 0) {
			spawnPowerup();
			m_spawnPowerupTimer = 0.0f;
		}
	

		
	
	

	

		// collisions
		PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
		while (it_projectiles.hasNext()) {
			Projectile* pr = it_projectiles.next();
			if (!pr->isPendingRemoval()) {

				if (pr->m_isFromPlayer) {
					if (pr->isInGround()) {
						if (m_player->m_bounds->collides(pr->m_bounds)) {
							pr->setPendingRemoval(true);
							m_player->m_numBoomerangs++;
							dg->m_sfxPlayerPickup->play();
						}
					}
					else { 

						PoolIterator<Enemy*> it_enemies = m_enemies->newiteratorref();
						while (it_enemies.hasNext()) {
							Enemy* e = it_enemies.next();
							if (!e->isPendingRemoval() && e->isDamagable() && e->m_bounds->collides(pr->m_bounds)) {
								pr->returnToPlayer();
								e->hit();
							}
						}

						PoolIterator<Powerup*> it_powerups = m_powerups->newiteratorref();
	    				while (it_powerups.hasNext()){
	    					Powerup* p = it_powerups.next();
	    					if (p->m_bounds->collides(pr->m_bounds)) {
	    						p->collected(m_player);
	    					}
	    				}

	    				PoolIterator<Pickup*> it_pickups2 = m_pickups->newiteratorref();
	    				while (it_pickups2.hasNext()){
	    					Pickup* p = it_pickups2.next();
	    					if (p->m_bounds->collides(pr->m_bounds)) {
	    						p->collected(m_player);
	    					}
	    				}

						if (pr->m_bounds->getMaxY() >= dg->m_floor + 5) {
							pr->m_bounds->setLocationByAlignment(pr->m_bounds->getCenterX(), dg->m_floor + 5, 0, 1);
							pr->m_travelTimer = pr->m_travelDuration;
							pr->m_to.set(pr->m_bounds->getCenterX(), pr->m_bounds->getCenterY());
							pr->m_grounded = true;
						}

					}
				}
				else {
					if (pr->m_bounds->collides(m_player->m_bounds)) {
						if (pr->m_type == PT_FLAME) {
							if (!m_player->m_burnProtection) {
								m_player->hitByProjectile(pr);
							}
						} else {
							m_player->hitByProjectile(pr);
						}
					}

					if (pr->m_bounds->getMaxY() >= dg->m_floor) {
						pr->setPendingRemoval(true);
						if (pr->m_type == PT_KISS) {
							for (float angle = 0; angle < 360; angle += 45) {
                                float bitX = pr->m_bounds->getCenterX();
								float bitY = pr->m_bounds->getCenterY();
								MathUtil::moveAngle<float>(bitX, bitY, angle, 5);
								MyParticle* particle = m_particles->get();
								particle->reset();
								particle->m_bounds->setLocationByCenter(bitX, bitY);
								particle->m_animation->addFrame(MyParticleSprites::s_cloud11);
								particle->m_color = Projectile::s_kissColor;
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
					}
				}
			}
		}
		// players on pickups
		PoolIterator<Pickup*> it_pickups = m_pickups->newiteratorref();
		while(it_pickups.hasNext()) {
			Pickup* pickup = it_pickups.next();
	        if (!pickup->isPendingRemoval()) {
	            if(pickup->m_grounded) {
	                if (pickup->m_bounds->collides(m_player->m_bounds) && !m_died) {
	                    pickup->collected(m_player);
	                }
	            } else {
	            	if (pickup->m_bounds->collides(m_player->m_bounds)) {
	                    m_player->hitByPickup(pickup);
	                }
	                if (pickup->m_bounds->getMaxY() > dg->m_floor) {
	                	pickup->m_bounds->setLocationByAlignment(pickup->m_bounds->getCenterX(), dg->m_floor, 0, 1);
	                	pickup->m_grounded = true;
	                	pickup->m_tumbling = false;
	                    pickup->m_velocity->set(0.0f, 0.0f);
	                }
	            }
	        }
		}

		// bombs, floor.
		PoolIterator<Bomb*> it_bombs = m_bombs->newiteratorref();
		while(it_bombs.hasNext()) {
			Bomb* bomb = it_bombs.next();
	        if (!bomb->isPendingRemoval()) {
	        	if (bomb->m_bounds->getMaxY() > dg->m_floor) {
	            	bomb->m_bounds->setLocationByAlignment(bomb->m_bounds->getCenterX(), dg->m_floor, 0, 1);
	            	bomb->m_grounded = true;
	            }
	            else if (!bomb->m_grounded) {
	            	if (m_player->m_bounds->collides(bomb->m_bounds)) {
						m_player->hitByBomb(bomb);
					}
	            }

	            // bombs on projectiles
	            PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
				while (it_projectiles.hasNext()) {
					Projectile* pr = it_projectiles.next();
					if (!pr->isPendingRemoval() && pr->m_isFromPlayer && !pr->isInGround()) {
						if (bomb->m_bounds->collides(pr->m_bounds)) {
							bomb->explode();
						}
					}
				}
	        }
	    }
	    // powerups on floor, and player.
	    PoolIterator<Powerup*> it_powerups = m_powerups->newiteratorref();
	    while (it_powerups.hasNext()){
	    	Powerup* p = it_powerups.next();
	    	if (p->m_bounds->getMaxY() >= dg->m_floor && !p->m_grounded) {
	    		p->m_grounded = true;
	    		p->m_velocity->set(0.0f, 0.0f);
	    		p->m_bounds->setLocationByAlignment(p->m_bounds->getCenterX(), dg->m_floor, 0, 1);
	    	}

	    	if (p->m_bounds->collides(m_player->m_bounds)) {
				p->collected(m_player);
	        }

	    }
	    
	    // enemies on player
	    PoolIterator<Enemy*> it_enemies = m_enemies->newiteratorref();
	    while(it_enemies.hasNext()) {
	    	Enemy* enemy = it_enemies.next();
	    	if (enemy->isPendingRemoval()) { continue; }
	    	if (enemy->m_bounds->collides(m_player->m_bounds)) {
	    		if (enemy->canHurtPlayer(m_player)) { 
	    			m_player->hitByEnemy(enemy);
	    		} else if (enemy->isDamagable() || 
	    					m_player->m_boots == Player::BOOTS_MOON ||
	    					m_player->m_boots == Player::BOOTS_HOVER) {
	    			enemy->die();
	    		}
	    	}
	    }
	}
    if (m_bombExplodedFilterTimer > 0.0f) {
		m_bombExplodedFilterTimer += timer->getDelta();
		if (m_bombExplodedFilterTimer >= m_bombExplodedFilterDuration) {
			m_bombExplodedFilterTimer = 0.0f;
		}
	}
	
	m_projectiles->pruneAll();
	m_enemies->pruneAll();
	m_bombs->pruneAll();
	m_pickups->pruneAll();
	m_powerups->pruneAll();
	m_particles->pruneAll();

	if (m_died || m_win || m_deadTimer > 0.0f) {
		m_deadTimer += timer->getDelta();
		if (m_deadTimer >= m_deadDuration) {
			dg->enterState(dg->stateSummary);
		}
	}
}
void InGameState::spawnSanta() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_SANTA);
}
void InGameState::spawnMole() {

	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_MOLE);
}
void InGameState::spawnMistletoe() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_MISTLETOE);
}
void InGameState::spawnPudding() {
	Enemy* e = m_enemies->get();
	e->reset();
	e->start(ET_CHRISTMASPUDDING);
}	
void InGameState::spawnPowerup() {
	GameContainer* container = ARK2D::getContainer();
	Powerup* p = m_powerups->get();
	p->reset();
    p->startRandom();
    p->m_bounds->setLocationByCenter(MathUtil::randBetweenf(20.0f, container->getWidth() - 20), MathUtil::randBetweenf(20, container->getHeight() * 0.33f));

	//p->
}
void InGameState::spawnBlood(float x, float y, float radius, int num) {
	spawnBlood(x, y, radius, num, 0, 360);
}
void InGameState::spawnBlood(float x, float y, float radius, int num, float minAngle, float maxAngle) {
	MyParticle* part = NULL;

	for(int i = 0; i < num; i++) {
		float splashStrength = MathUtil::randBetweenf(30.0f, 60.0f); 
		float tx = x;
		float ty = y;
		MathUtil::moveAngle<float>(tx, ty, MathUtil::randBetweenf(0,360), MathUtil::randBetweenf(0.0f, radius));

		Image* frame = MyParticleSprites::s_bloodSelection->at(MathUtil::randBetween(0, MyParticleSprites::s_bloodSelection->size()));
		part = m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(frame);
		part->m_type = MyParticle::TYPE_BLOOD;
		part->m_color = Color::red.pointer();
		part->m_bounds->setLocationByCenter(tx, ty);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(minAngle,maxAngle), splashStrength);
	}
}

void InGameState::smartBomb() {
	m_flashTimer = 0.01f;
	
	PoolIterator<Enemy*> it_enemies  = m_enemies->newiteratorref();
	while (it_enemies.hasNext()) {
		Enemy* e = it_enemies.next();
		if (!e->isPendingRemoval() && e->killableBySmartBomb()) {
			e->die();
		}
	}

	PoolIterator<Projectile*> it_projectiles = m_projectiles->newiteratorref();
	while( it_projectiles.hasNext()) {
		Projectile* pr = it_projectiles.next();
		if (!pr->isPendingRemoval()) {
			if (pr->m_type == PT_KISS || pr->m_type == PT_FLAME) {
				pr->setPendingRemoval(true);
			}
		}
	}
}

void InGameState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	r->pushMatrix();
	r->translate(m_cameraShake->getXOffset(), m_cameraShake->getYOffset());

	DefaultGame* dg = DefaultGame::getInstance();
	r->setDrawColor(Color::white);
	//r->drawString("In Game State", 20, 20);

	dg->renderBackground();

	
	m_bombs->renderAll(container, r);
	m_player->render(container, r);
	m_projectiles->renderAll(container, r);
	m_enemies->renderAll(container, r);
	m_pickups->renderAll(container, r);
	m_powerups->renderAll(container, r);
	m_particles->renderAll(container, r);

	// STOP SHAKING IT
	r->popMatrix();

	

	// draw UI here
	float uiScale = 1.0f;
	if (m_introTimer > 0.0f) {
		uiScale = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	if (m_deadTimer > 0.0f) {
		uiScale = Easing::easebetween(Easing::QUADRATIC_OUT, m_deadTimer, 1.0f, 0.0f, m_deadDuration);
	}
	r->setDrawColorf(1.0f,1.0f,1.0f,uiScale);
	dg->font->drawString(Cast::toString<unsigned int>(m_score), (container->getWidth()*0.5f)-2, 20*uiScale, Renderer::ALIGN_CENTER,Renderer::ALIGN_CENTER, 0.0f, 2.0f * uiScale);

	if (m_currentPowerup != NULL) {
		m_currentPowerupContainer->drawCenteredScaled(20, 20*uiScale, uiScale, uiScale);
		m_currentPowerup->drawCenteredScaled(20, 20 * uiScale, uiScale, uiScale);
	}

	if (m_flashTimer > 0.0f) {
		float alpha = Easing::easebetween(Easing::QUADRATIC_OUT, m_flashTimer, 1.0f, 0.0f, m_flashDuration);
		r->setDrawColorf(1.0f, 1.0f, 1.0f, alpha);
		r->fillRect(0,0,container->getWidth(), container->getHeight());
	}
}

InGameState::~InGameState() {

}

