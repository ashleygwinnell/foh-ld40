/*
 * MyParticle.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "MyParticle.h"
#include "../DefaultGame.h"
  
#include "Player.h"
#include "Projectile.h"
//#include "Explosion.h"
//#include "Enemy.h"

Image* MyParticleSprites::s_lineShort = NULL;
Image* MyParticleSprites::s_lineMedium = NULL;
Image* MyParticleSprites::s_lineLong = NULL;

Image* MyParticleSprites::s_cloud11 = NULL;
Image* MyParticleSprites::s_cloud22 = NULL;
Image* MyParticleSprites::s_cloud33 = NULL;
Image* MyParticleSprites::s_cloud44 = NULL;
Image* MyParticleSprites::s_cloud55 = NULL;
Image* MyParticleSprites::s_cloud77 = NULL;
vector<Image* >* MyParticleSprites::s_bloodSelection = NULL;

void MyParticleSprites::init() {
    DefaultGame* dg = DefaultGame::getInstance();
	if (s_lineShort == NULL) {
		
		s_lineShort = SpriteSheetStore::getImage("sprites/particles/line-short.png");
		s_lineMedium = SpriteSheetStore::getImage("sprites/particles/line-medium.png");
		s_lineLong = SpriteSheetStore::getImage("sprites/particles/line-long.png");

		s_cloud11 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/particles/cloud-1x1.png"));
		s_cloud11->setCenterOfRotation(s_cloud11->getWidth()/2, s_cloud11->getHeight()*0.5f);

		s_cloud22 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/particles/cloud-2x2.png"));
		s_cloud22->setCenterOfRotation(s_cloud22->getWidth()/2, s_cloud22->getHeight()*0.5f);

		s_cloud33 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/particles/cloud-3x3.png"));
		s_cloud33->setCenterOfRotation(s_cloud33->getWidth()/2, s_cloud33->getHeight()*0.5f);

		s_cloud44 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/particles/cloud-4x4.png"));
		s_cloud44->setCenterOfRotation(s_cloud44->getWidth()/2, s_cloud44->getHeight()*0.5f);

		s_cloud55 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/particles/cloud-5x5.png"));
		s_cloud55->setCenterOfRotation(s_cloud55->getWidth()/2, s_cloud55->getHeight()*0.5f);

		s_cloud77 = dg->spritesheet->getSubImage(dg->desc->getItemByName("sprites/particles/cloud-7x7.png"));
		s_cloud77->setCenterOfRotation(s_cloud77->getWidth()/2, s_cloud77->getHeight()*0.5f);

		s_bloodSelection = new vector<Image*>();
		s_bloodSelection->push_back(s_cloud11);
		s_bloodSelection->push_back(s_cloud22);
		s_bloodSelection->push_back(s_cloud33);
		s_bloodSelection->push_back(s_cloud44);
	}
}

MyParticle::MyParticle(): 
	Object()
	{

	DefaultGame* dg = DefaultGame::getInstance(); 

	MyParticleSprites::init();

    m_bounds = new ARK::Core::Geometry::Rectangle(0, 0, 5, 5);
	m_bounds->setLocationByCenter(160, 50);
 
	m_velocity->set(0, 0);
	m_velocityMax->set(400, 400);
	m_acceleration->set(0, 0);

	m_animation = new Animation();
	m_animation->setFrameTime(0.1f);
 
 	reset();
}   

    
void MyParticle::reset() {   
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;
	m_velocity->set(0, 0);

	m_lifetime = false;

	m_lifeTimer = 0.0f;
	m_lifeDuration = 1.0f;

	m_lifeScaleStart = 1.0f;
	m_lifeScaleEnd = 0.0f;

	m_lifeAlphaStart = 1.0f;
	m_lifeAlphaEnd = 0.0f;

	m_rotateToVelocity = true;
	m_rotationOffset = 0.0f;
	
	m_useGravity = true;

	m_animation->clear();
	m_color = NULL;
	//m_bounds->setLocationByCenter(igs->m_centerOfGravity->getX(), igs->m_centerOfGravity->getY());
}  

void MyParticle::start(unsigned int type) {
	m_type = type;
	
	
	
	if (type == TYPE_PLAYER_GIB) {
		m_bounds->asRectangle()->setSize(8, 8);

		m_rotateToVelocity = true;
		m_lifetime = true;

		m_lifeDuration = 4.0f;

		m_lifeScaleStart = 1.0f;
		m_lifeScaleEnd = 1.0f;

		m_lifeAlphaStart = 1.0f;
		m_lifeAlphaEnd = 0.0f;
	}
	
	else if (type == TYPE_EXPLOSION_CIRCLE || type == TYPE_BOMB_CIRCLE) {
		//m_sprite = NULL;

		//m_bounds->asRectangle()->setSize(Enemy::explosionRadius, Enemy::explosionRadius);

		m_lifetime = true;
		m_lifeDuration = 0.8f;

		m_lifeScaleStart = 1.0f;
		m_lifeScaleEnd = 0.0f;

		m_lifeAlphaStart = 1.0f;
		m_lifeAlphaEnd = 0.0f;

		m_useGravity = false;

		m_animation->addFrame(MyParticleSprites::s_cloud11);
		m_velocity->set(0.0f, 0.0f);
		
	}
	else if (type == TYPE_PRESENT_COLLECT) 
	{
		m_lifetime = true;
		m_lifeDuration = 1.0f;

		m_lifeScaleStart = 1.0f;
		m_lifeScaleEnd = 0.0f;

		m_lifeAlphaStart = 1.0f;
		m_lifeAlphaEnd = 0.0f;

		m_useGravity = false;
		m_rotateToVelocity = false;
	}
}


void MyParticle::update(GameContainer* container, GameTimer* timer) {

	if (m_lifetime) {
		m_lifeTimer += timer->getDelta();
		if (m_lifeTimer >= m_lifeDuration) {
			setPendingRemoval(true);
		}
	}

	if (m_useGravity) {
		Object::gravity(1.0f);
	}

	move(1.0f);

}

void MyParticle::collision(Object* other) { 
	DefaultGame* dg = DefaultGame::getInstance();
	InGameState* igs = dg->stateInGame;

	Player* p = NULL;
	p = dynamic_cast<Player*>(other);
	if (p) {
		//igs->m_score += 1;
		setPendingRemoval(true);
		return; 
	}  

	Projectile* pr = NULL;
	pr = dynamic_cast<Projectile*>(other);
	if (pr) {
		//igs->m_score += 1;
		setPendingRemoval(true);
		return;
	}  
}  

void MyParticle::render(GameContainer* container, Renderer* r) {
	//r->setDrawColor(Color::cyan); 
	//r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), m_bounds->getWidth()/2, 20);
	//r->fillRect(m_bounds->getMinX(), m_bounds->getMinY(), m_bounds->getWidth(), m_bounds->getHeight());
	DefaultGame* dg = DefaultGame::getInstance();
	r->setDrawColor(Color::white);
	
	float scale = 1.0f;
	float alpha = 1.0f;
	if (m_lifetime) {
		scale = Easing::easebetween(Easing::QUADRATIC_IN, m_lifeTimer, m_lifeScaleStart, m_lifeScaleEnd, m_lifeDuration);
		alpha = Easing::easebetween(Easing::QUADRATIC_IN, m_lifeTimer, m_lifeAlphaStart, m_lifeAlphaEnd, m_lifeDuration);
	}

	float rotation = m_rotationOffset;
	if (m_rotateToVelocity) {
		rotation = MathUtil::anglef(0.0f, 0.0f, m_velocity->getX(), m_velocity->getY()) + 90 + m_rotationOffset;
	}
	if (m_type == TYPE_PLAYER_GIB || m_type == TYPE_BLOOD  || m_type == TYPE_SMOKE) {
		rotation = MathUtil::anglef(0,0,m_velocity->getX(), m_velocity->getY());
	}

	if (m_type == TYPE_ENERGY) {
		//r->setDrawColor(dg->m_energyColor);
	}

	if (m_color != NULL) {
		r->setDrawColor(m_color);
	}

	Image* img = m_animation->getCurrentFrame();
	if (m_type == TYPE_TEXT) {
		r->setDrawColorf(1.0f, 1.0f, 1.0f, alpha);
		dg->font->drawString(m_text, m_bounds->getCenterX(), m_bounds->getCenterY(), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, rotation, scale);
	}
	else if (m_type == TYPE_EXPLOSION_CIRCLE || m_type == TYPE_BOMB_CIRCLE) {
		
		r->setDrawColorf(1.0f, 1.0f, 1.0f, alpha);
		r->fillCircle(m_bounds->getCenterX(), m_bounds->getCenterY(), m_bombCircleRadius * alpha, 40);
		
	}
	else {
        if (img != NULL) {
            img->setAlpha(alpha);
            img->setRotation(rotation);
            img->drawCenteredScaled(m_bounds->getCenterX(), m_bounds->getCenterY(), scale, scale);
        }
	}
}

MyParticle::~MyParticle() { 
	 
}
