/*
 * Object.cpp
 *
 *  Created on: 18 April 2012
 *      Author: Ashley
 */

#include "Object.h"
#include "../DefaultGame.h"
#include "MyParticle.h"

float Object::s_gravity = 250.0f; 

Object::Object(): 
	GameObject(), 
	m_bounds(NULL),
	m_boundsOnCamera(NULL)
	{ 
	m_velocity = new Vector2<float>(0,0);
	m_velocityMax = new Vector2<float>(0,0);
	m_acceleration = new Vector2<float>(0,0);
	m_gravity = new Vector2<float>(0,0);

	m_restrictVelocity = true;
}

void Object::update(GameContainer* container, GameTimer* timer) {
//	m_gravity

} 

void Object::teleport() {
	m_bounds->setLocationByCenter( 
		MathUtil::randBetweenf(30.0f, ARK2D::getContainer()->getWidth() - 30.0f),
		MathUtil::randBetweenf(70.0f, ARK2D::getContainer()->getHeight() - 70.0f)
	); 
	keepInBounds();
} 

void Object::render(GameContainer* container, Renderer* r) {
	if (ARK2D::isDebug() && m_bounds != NULL) { 
		m_bounds->render(); 
	}
}

void Object::fireParticles() {
    DefaultGame* dg = DefaultGame::getInstance();
    InGameState* igs = dg->stateInGame;

	MyParticle* part = NULL;
	
	part = igs->m_particles->get();
	part->reset();
	part->m_animation->addFrame(MyParticleSprites::s_cloud55);
    part->m_lifetime = true;
    part->m_lifeDuration = MathUtil::randBetweenf(1.0f, 2.0f);
	part->m_type = MyParticle::TYPE_SMOKE;
	part->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	part->m_velocity->set(0.0f, 0.0f);
	MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-135.0f, -45.0f), MathUtil::randBetweenf(10.0f, 30.0f));

	part = igs->m_particles->get();
	part->reset();
	part->m_animation->addFrame(MyParticleSprites::s_cloud33);
    part->m_lifetime = true;
	part->m_lifeDuration = MathUtil::randBetweenf(1.0f, 2.0f);
	part->m_type = MyParticle::TYPE_SMOKE;
	part->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	part->m_velocity->set(0.0f, 0.0f);
	MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-135.0f, -45.0f), MathUtil::randBetweenf(10.0f, 30.0f));

	part = igs->m_particles->get();
	part->reset();
	part->m_animation->addFrame(MyParticleSprites::s_cloud22);
    part->m_lifetime = true;
	part->m_lifeDuration = MathUtil::randBetweenf(1.0f, 2.0f);
	part->m_type = MyParticle::TYPE_SMOKE;
	part->m_bounds->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY());
	part->m_velocity->set(0.0f, 0.0f);
	MathUtil::moveAngle<float>(part->m_velocity, MathUtil::randBetweenf(-135.0f, -45.0f), MathUtil::randBetweenf(10.0f, 30.0f));
	
}

  
bool Object::keepInBounds() 
{
	GameContainer* container = ARK2D::getContainer();
	ARK::Core::Geometry::Rectangle* bounds = m_bounds->asRectangle();

	int leftX = 0;
	int rightX = container->getWidth();

	bool b = false;
	if (bounds->getMinX() < leftX) {
		b = true;
		bounds->setX(leftX); 
		m_velocity->setX(m_velocity->getX() * -1);
	} else if (bounds->getMaxX() > rightX) {
		b = true;
		bounds->setX(rightX - bounds->getWidth());
		m_velocity->setX(m_velocity->getX() * -1);
	}
	return b;
	
	// Keep in bounds.
	// bool b = false;
	// if (bounds->getMinX() < 0) {
	// 	b = true;
	// 	bounds->setX(0); 
	// 	//m_velocity->setX(m_velocity->getX() * -1);
	// } else if (bounds->getMaxX() > container->getWidth()) {
	// 	b = true;
	// 	bounds->setX(container->getWidth() - bounds->getWidth());
	// 	//m_velocity->setX(m_velocity->getX() * -1);
	// }
	// // if (bounds->getMinY() < 58) {
	// // 	b = true; 
	// // 	bounds->setY(58); 
	// // 	//m_velocity->setY(m_velocity->getY() * -1);
	// // } else 
	// if (bounds->getMaxY() > container->getHeight() - 40) {  
	//  	b = true; 
	//  	bounds->setY(container->getHeight() - bounds->getHeight() - 40); 
	//  	//m_velocity->setY(m_velocity->getY() * -1);
	//  }
	// return b;
	//return false;
}
void Object::gravity() {
	gravity(1.0f); 
}
void Object::gravity(float multiplier) {
	// Add Gravity 
	GameTimer* timer = ARK2D::getContainer()->getTimer();
	
	/*InGameState* igs = DefaultGame::getInstance()->stateInGame;
	float x = 0.0f;
	float y = 0.0f;
	float angle = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_centerOfGravity->getX(), igs->m_centerOfGravity->getY());
	//float distance = MathUtil::anglef(m_bounds->getCenterX(), m_bounds->getCenterY(), igs->m_centerOfGravity->getX(), igs->m_centerOfGravity->getY());
	MathUtil::moveAngle<float>(x, y, angle, s_gravity * multiplier * timer->getDelta());

	m_velocity->add(x, y);   */

	m_velocity->add(0, s_gravity * multiplier * timer->getDelta()); 
	
	if (m_restrictVelocity) { 
		m_velocity->restrictMax(m_velocityMax);
	}
}

void Object::move(float multiplier = 1) {
	GameTimer* timer = ARK2D::getContainer()->getTimer();
    ARK::Core::Geometry::Rectangle* bounds = m_bounds->asRectangle();
	bounds->adjustX(m_velocity->getX() * multiplier * timer->getDelta());
	bounds->adjustY(m_velocity->getY() * multiplier * timer->getDelta());
	onBoundsMoved();
}
void Object::onBoundsMoved() {
	if (m_boundsOnCamera != NULL) {
		InGameState* igs = DefaultGame::getInstance()->stateInGame;
		//m_boundsOnCamera->setLocationByCenter(m_bounds->getCenterX(), m_bounds->getCenterY() - igs->m_cameraSnapY);
	}
}

Object::~Object() {
	delete m_velocity;
	delete m_velocityMax;
	delete m_gravity;
}
