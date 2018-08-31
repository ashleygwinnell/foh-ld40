/*
 * Player.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "Player.h"
#include "../DefaultGame.h"

#include "Projectile.h"
#include "MyParticle.h"
#include "EightWayMovementComponent.h"
#include "Bomb.h"
#include "Enemy.h"
#include "Powerup.h"
//#include "../states/UpgradesState.h"

#include <ARK2D/Core/Tween/Timer.h>
#include <ARK2D/Util/CameraShake.h>

Image* Player::s_playerGibHead = NULL;
Image* Player::s_playerGibBody = NULL;
Image* Player::s_playerGibHand = NULL;
Image* Player::s_playerGibWeapon = NULL;

Image* Player::s_playerBootsMoon = NULL;
Image* Player::s_playerBootsHover = NULL;

Image* Player::s_playerHalo = NULL;

Player::Player(): 
	Object(),
	m_fireTimer(NULL)
	{

	DefaultGame* dg = DefaultGame::getInstance(); 

	if (s_playerGibHead == NULL) {
		s_playerGibHead = SpriteSheetStore::getImage("sprites/carl-gib-head.png");
		s_playerGibBody = SpriteSheetStore::getImage("sprites/carl-gib-body.png");
		s_playerGibHand = SpriteSheetStore::getImage("sprites/carl-gib-hand.png");
		s_playerGibWeapon = SpriteSheetStore::getImage("sprites/boomerang-hand.png");
		
		s_playerBootsMoon = SpriteSheetStore::getImage("sprites/carl-moon-boots.png");
		s_playerBootsHover = SpriteSheetStore::getImage("sprites/carl-hoverboard.png");
		
		s_playerHalo = SpriteSheetStore::getImage("sprites/carl-halo.png");
	}

	AnimationFrameList* idleFrames = AnimationFrameList::create("sprites/carl-idle-*.png", 1, 2, dg->spritesheet, dg->desc);
        
	m_animationIdle = new Animation();
	m_animationIdle->setFrames(idleFrames);
	m_animationIdle->setFrameTime(0.5f);

	m_boomerangHand = SpriteSheetStore::getImage("sprites/boomerang-hand.png");

	m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 8.0f, 7.0f);
	
	m_velocity->set(0, 0);  
	m_velocityMax->set(40, 200);    
	m_acceleration->set(600, 600);

	m_fireTimer = new Timer(0.20f);

	m_movement = new EightWayMovementComponent(this);
	m_movement->setController(true, 0, Gamepad::ANALOG_STICK_1_X);
	m_movement->setKeysWASD();
	
	reset();
} 

void Player::hitByProjectile(Projectile* pr) {
	pr->setPendingRemoval(true);
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
}
void Player::hitByPickup(Pickup* pu) {
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
}
void Player::hitByBomb(Bomb* b) {
	m_lives++;
	b->explode();

	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
}
void Player::hitByBombExplode(Bomb* b) {
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}

}
void Player::hitByEnemy(Enemy* e) {
	m_lives--;
	onLoseLife();
	if (m_lives == 0) {
		die();
	}
	e->die();	
}
void Player::onLoseLife() {
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	if (m_lives == 1 && igs->m_currentPowerupType == PT_LIVES_X2) {
		dg->m_sfxPlayerHurt->play();

		igs->m_currentPowerupType = PT_NONE;
		igs->m_currentPowerup = NULL;
	}
}
   
void Player::reset() {  
	DefaultGame* dg = DefaultGame::getInstance();
	GameContainer* container = ARK2D::getContainer();

	m_bounds->setLocationByCenter(container->getWidth()*0.5, -10.0f);//dg->m_floor - (m_bounds->getHeight()*0.5f));

	m_numBoomerangs = 1;
	m_boomerangSpeedModifier = 1.0f;
	m_burnProtection = false;

	m_moving = false;
	m_grounded = false;
	m_facingLeft = false;
	m_rotation = 0.0f;

	m_introTimer = 0.01f;
	m_introDuration = 0.8f;

	m_boots = BOOTS_NONE;
	m_lives = 1;



	m_velocity->set(0, 0);
	m_fireTimer->reset();

	m_dieReason = DIE_NULL;
}  
void Player::setBoots(int boots) {
	m_boots = boots;

	if (m_boots == BOOTS_NONE) {
		m_velocityMax->setX(40);
		m_bounds->asRectangle()->setSize(8.0f, 7.0f);
	} else if (m_boots == BOOTS_MOON) {
		m_velocityMax->setX(50);
		m_bounds->asRectangle()->setSize(8.0f, 9.0f);
	} else if (m_boots == BOOTS_HOVER) {
		m_velocityMax->setX(75);
		m_bounds->asRectangle()->setSize(8.0f, 9.0f);
	}


}

void Player::update(GameContainer* container, GameTimer* timer) {

	

	// Movement
	Object::gravity(1.0f);
	
	// Do friction towards gravity.
	// float frictionX = 0.0f;  
	// float frictionY = 0.0f;
	// float frictionAngle = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY(), true) - 180;
	// float currentAngle = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY(), true);
	// MathUtil::moveAngle<float>(m_velocity, frictionAngle, 150.0f * timer->getDelta());
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = DefaultGame::getInstance()->stateInGame;
	Input* in = ARK2D::getInput();
	//GameContainer* container = ARK2D::getContainer();
	//if (in->isKeyDown(Input::KEY_A) || in->isKeyDown(Input::KEY_LEFT)) {
	//	m_velocity->m_x -= m_acceleration->getX() * timer->getDelta();
	//}
	m_movement->update(container, timer);

	float angle = m_movement->m_lastAngle;
	bool keyDown = m_movement->m_stickPushing || m_movement->m_buttonPushing;

	if (m_movement->isLeftDown() && m_movement->isRightDown()) {

	} else if (m_movement->isLeftDown()) {
		m_facingLeft = true;
	} else if (m_movement->isRightDown()) {
		m_facingLeft = false;
	} else {
		
	}
	//m_facingLeft = (in->getMouseX() <= m_bounds->getCenterX());

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer >= m_introDuration){
			m_introTimer = 0.0f;
		}
	}

	if (m_bounds->getMaxY() > dg->m_floor) {
		m_grounded = true;
		m_rotation = 0.0f;
		m_velocity->m_y = 0;
		m_bounds->setLocationByCenter(m_bounds->getCenterX(), dg->m_floor - (m_bounds->getHeight()*0.5f));
	}

	if (m_movement->isUpDown() && m_grounded) {
		m_grounded = false;

		dg->m_sfxPlayerJump->play();

		float jumpVel = (m_boots == BOOTS_MOON)?-150:-100;
		m_velocity->setY(jumpVel);
	}

	if (!m_grounded) {
		m_rotation += 450.0f * timer->getDelta();
	}

	

	
		if (keyDown) {
			m_animationIdle->setFrameTime(0.25f);
			m_moving = true;

			float distance = 1.0f;
			float maxAcceleration = 800.0f;

			m_acceleration->set(0.0f, 0.0f);
			MathUtil::moveAngle<float>(m_acceleration, angle, maxAcceleration);

			m_velocity->addX(m_acceleration->getX() * timer->getDelta());
			//m_velocity->addY(m_acceleration->getY() * timer->getDelta());
			m_velocity->restrictLength(m_velocityMax->length());
		} else {
			m_animationIdle->setFrameTime(0.5f);

			m_moving = false;
			m_acceleration->set(0.0f, 0.0f);
			
			m_velocity->frictionX(600 * timer->getDelta());
			//m_velocity->frictionY(600 * timer->getDelta());

			//m_walkWobbleTimer = 0.0f;

			//m_animationIdle->setFrameTime(0.5f);
			//m_animationIdleBack->setFrameTime(0.5f);
		}

		//m_velocity->m_y = 0;
	

	if (!igs->m_died && in->isKeyPressed(Input::MOUSE_BUTTON_LEFT) && m_numBoomerangs > 0) {
		Projectile* p = igs->m_projectiles->get();
		p->reset();
		p->start(PT_BOOMERANG);
		Vector2<float> handBoom = getHandPosition();
		p->m_bounds->setLocationByCenter(handBoom.getX(), handBoom.getY());
		p->m_from.set(handBoom.getX(), handBoom.getY());
		p->m_to.set(in->getMouseX(), in->getMouseY());
		p->m_travelRotation = (m_facingLeft)?-90:0;

		p->updatePlayerBoomerangDuration();
		
		dg->m_sfxPlayerThrow->play();
		

		m_numBoomerangs--;
	}


	//if (in->isKeyPressed(Input::KEY_I)) {
		//m_numBoomerangs++;
	//}

	//m_velocity->set(0.0f, 0.0f);

	Object::move(1.0f); 
	bool b = keepInBounds();
	if (b) {
		//igs->m_cameraShake->setSpeedMultiplier(1, 1.0f);
		//igs->m_cameraShake->start(1, 2.0f);
	}

	m_animationIdle->update(timer);

	// Set rotation of player for velocity
	//m_rotation = MathUtil::anglef(0,0, m_velocity->getX(), m_velocity->getY()) + 90; 
} 
void Player::collision(Object* other) {  

} 
void Player::die() {

	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	if (!igs->m_died) {
		dg->m_sfxPlayerDie->play();
		

		if (dg->m_musicGame->isPlaying()) { 
			dg->m_musicGame->setLowpass(true); 
			dg->m_musicGame->setLowpassFrequency(500.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
		}



		igs->spawnBlood(m_bounds->getCenterX(), m_bounds->getCenterY(), 5, 10, 190, 350);
		igs->m_died = true;

		// player giblets
		MyParticle* part;
		float strengthMin = 60.0f;
		float strengthMax = 120.0f;
		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibHead);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY()-2);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-120,-60), MathUtil::randBetweenf(strengthMin, strengthMax));

		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibBody);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY()+1);
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-120,-60), MathUtil::randBetweenf(strengthMin, strengthMax));

		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibHand);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX() - 3, m_bounds->getCenterY());
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-135,-45), MathUtil::randBetweenf(strengthMin, strengthMax));

		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibHand);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(m_bounds->getCenterX() + 3, m_bounds->getCenterY());
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-135,-45), MathUtil::randBetweenf(strengthMin, strengthMax));

		part = igs->m_particles->get();
		part->reset();
		part->m_animation->reset();
		part->m_animation->addFrame(s_playerGibWeapon);
		part->m_type = MyParticle::TYPE_PLAYER_GIB;
		part->m_bounds->setLocationByCenter(getHandPosition().getX(), getHandPosition().getY());
		MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-135,-45), MathUtil::randBetweenf(strengthMin, strengthMax));
	}

}
Vector2<float> Player::getHandPosition() {
	//float facingMultiplier = (m_facingLeft)?1:-1;
	//float handX = m_bounds->getCenterX() + (6*facingMultiplier);
	//float handY = m_bounds->getCenterY() - 0.5f;
	
	float facingMultiplier = (m_facingLeft)?-1:1;
	float handX = m_bounds->getCenterX();
	float handY = m_bounds->getCenterY();
	MathUtil::moveAngle<float>(handX, handY, ((m_rotation+90)*facingMultiplier)-90, 6);
	
	return Vector2<float>(handX, handY);
}

void Player::render(GameContainer* container, Renderer* r) 
{  
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	if (igs->m_died) {
		return;
	}	


	float scale = 1.0f;
	if (m_introTimer > 0.0f) {
		scale = Easing::easebetween(Easing::QUADRATIC_IN, m_introTimer, 4.0f, 1.0f, m_introDuration);
	}
//    if (m_bounceTimer->getTime() > 0.0f) {
//        scale *= Easing::easebetween(Easing::QUADRATIC_OUT, m_bounceTimer->getTime(), 1.2f, 1.0f, m_bounceTimer->getDuration());
//    }
	r->setDrawColor(Color::white);

	//float rot = m_rotation;
	if (m_grounded) {
		//rot = 0;
	}

	
	Image* img = m_animationIdle->getCurrentFrame();
	if (m_facingLeft) { 
		img->flip(true, false); 
		img->setRotation(m_rotation*-1);
	} else {
		img->setRotation(m_rotation);	
	}
	
	float playerOffsetY = (m_boots != BOOTS_NONE)?-2:0;

	img->drawAligned(m_bounds->getCenterX(), m_bounds->getMaxY()-0.5f+playerOffsetY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale);

	Image* bootsSprite = NULL;
	if (m_boots == BOOTS_MOON) {
		bootsSprite = s_playerBootsMoon;
	} else if (m_boots == BOOTS_HOVER) {
		bootsSprite = s_playerBootsHover;
	}
	float bootsX = m_bounds->getCenterX();
	float bootsY = m_bounds->getCenterY();
	if (bootsSprite != NULL) {
		float facingMultiplier = (m_facingLeft)?-1:1;
		//if (!m_facingLeft) {
        	MathUtil::moveAngle<float>(bootsX, bootsY, (m_rotation*facingMultiplier)+90, (m_bounds->getHeight()*0.5f));
			bootsSprite->setRotation(m_rotation*facingMultiplier);
			bootsSprite->drawAligned(bootsX, bootsY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale);
   //      } else {
   //      	MathUtil::moveAngle<float>(bootsX, bootsY, (m_rotation*-1)+90, (m_bounds->getHeight()*0.5f));
			// bootsSprite->setRotation(m_rotation*-1);
			// bootsSprite->drawAligned(bootsX, bootsY, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, scale);
   //      }
	}

	if (m_lives > 1) {
		float haloX = m_bounds->getCenterX();
		float haloY = m_bounds->getCenterY();
		float facingMultiplier = (m_facingLeft)?-1:1;
        MathUtil::moveAngle<float>(haloX, haloY, (m_rotation*facingMultiplier)-90, (m_bounds->getHeight()*0.5f) + 2);
        s_playerHalo->setRotation(m_rotation*facingMultiplier);
		s_playerHalo->drawCenteredScaled(haloX, haloY, scale, scale);
	}
	
	
	if (m_numBoomerangs > 0) { 
		Vector2<float> handBoom = getHandPosition();

		if (!m_facingLeft) { m_boomerangHand->flip(true, false); }
		float facingMultiplier = (m_facingLeft)?-1:1;
		m_boomerangHand->setRotation(m_rotation*facingMultiplier);
		m_boomerangHand->drawCenteredScaled(handBoom.getX(), handBoom.getY(), scale, scale);
		if (!m_facingLeft) { m_boomerangHand->flip(true, false); }
	}

	if (m_facingLeft) { img->flip(true, false); }

	//m_face->setRotation(rot);
	//m_face->drawCenteredScaled(m_bounds->getCenterX(), m_bounds->getCenterY(), scale, scale);
	//r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), (m_bounds->getWidth()/2.0f) * scale, 20);
}

Player::~Player() { 
	delete m_fireTimer;
}
