/*
 * EightWayMovementComponent.cpp
 *
 *  Created on: 29 Jan 2011
 *      Author: Ashley
 */

#include "EightWayMovementComponent.h"
#include "../DefaultGame.h"

EightWayMovementComponent::EightWayMovementComponent(Object* o):
	m_parent(o),

	m_velocity(NULL),
	m_lastAngle(0.0f),

	m_controllerInput(false),
	m_controllerIndex(-1),
	m_controllerAxisIndex(Gamepad::ANALOG_STICK_1_X),

	m_keyUp(Input::KEY_UP),
	m_keyDown(Input::KEY_DOWN),
	m_keyLeft(Input::KEY_LEFT),
	m_keyRight(Input::KEY_RIGHT),

	m_controllerUp(Gamepad::DPAD_UP),
	m_controllerDown(Gamepad::DPAD_DOWN),
	m_controllerLeft(Gamepad::DPAD_LEFT),
	m_controllerRight(Gamepad::DPAD_RIGHT),

	m_snap(false),

	m_stickPushing(false),
	m_buttonPushing(false)
{
	m_velocity = new Vector2<float>(0, 0);
}

void EightWayMovementComponent::reset() {
	m_velocity->set(0, 0);
	m_lastAngle = 0.0f;
}

void EightWayMovementComponent::setController(bool b, unsigned int controllerIndex, unsigned int controllerAxisIndex) {
	m_controllerInput = b;

	if (b) {
		m_controllerIndex = controllerIndex;
		m_controllerAxisIndex = controllerAxisIndex;
	} else {
		m_controllerIndex = -1;
		m_controllerAxisIndex = -1;
	}


}
void EightWayMovementComponent::setKeys(unsigned int up, unsigned int down, unsigned int left, unsigned int right) {
	m_keyUp = up;
	m_keyDown = down;
	m_keyLeft = left;
	m_keyRight = right;
}

bool EightWayMovementComponent::isUpDown() {
	return (m_stickPushing && (
				(m_lastAngle <= 0 && m_lastAngle >= -180) ||
				(m_lastAngle >= 180 && m_lastAngle <= 360) )) ||
			(m_buttonPushing && (
				(m_lastAngle <= -45 && m_lastAngle >= -135) ||
				(m_lastAngle >= 225 && m_lastAngle <= 315)
			));
}
bool EightWayMovementComponent::isDownDown() {
	return (m_stickPushing && m_lastAngle >= 0 && m_lastAngle <= 180) ||
		   (m_buttonPushing && m_lastAngle >= 45 && m_lastAngle <= 135);
}
bool EightWayMovementComponent::isLeftDown() {
	return (m_stickPushing || m_buttonPushing) && (
				(m_lastAngle <= -90 && m_lastAngle >= -270) ||
				(m_lastAngle >= 90 && m_lastAngle <= 270)
			);
}
bool EightWayMovementComponent::isRightDown() {
	return (m_stickPushing || m_buttonPushing) && (
				(m_lastAngle >= -90 && m_lastAngle <= 90) ||
				(m_lastAngle >= 270 && m_lastAngle <= 450)
			);
}
bool EightWayMovementComponent::isJumpPressed() {
	return (m_controllerInput &&
			m_controllerIndex >= 0 &&
			ARK2D::getInput()->getGamepad(m_controllerIndex) != NULL &&
			ARK2D::getInput()->getGamepad(m_controllerIndex)->isButtonPressed(Gamepad::BUTTON_A))
		? true
		: ARK2D::getInput()->isKeyPressed(Input::KEY_UP);
}

void EightWayMovementComponent::update(GameContainer* container, GameTimer* timer) {

	Input* in = ARK2D::getInput();

	m_velocity->set(0, 0);

	float x = 0;
	float y = 0;


		bool keyDown = false;
		float angle = 0;
		if (in->isKeyDown(m_keyUp) && in->isKeyDown(m_keyDown)) {

		}
		else if (in->isKeyDown(m_keyLeft) && in->isKeyDown(m_keyRight)) {
			
		} 
		else if (in->isKeyDown(m_keyUp) && in->isKeyDown(m_keyLeft)) {
			angle = -135;
			keyDown = true;
		} else
		if (in->isKeyDown(m_keyDown) && in->isKeyDown(m_keyLeft)) {
			angle = 135;
			keyDown = true;
		} else if (in->isKeyDown(m_keyUp) && in->isKeyDown(m_keyRight)) {
			angle = -45;
			keyDown = true;
		}  else if (in->isKeyDown(m_keyDown) && in->isKeyDown(m_keyRight)) {
			angle = 45;
			keyDown = true;
		} else if (in->isKeyDown(m_keyUp)) {
			angle = -90;
			keyDown = true;
		} else if (in->isKeyDown(m_keyDown)) {
			angle = 90;
			keyDown = true;
		} else if (in->isKeyDown(m_keyLeft)) {
			angle = 180;
			keyDown = true;
		} else if (in->isKeyDown(m_keyRight)) {
			angle = 0;
			keyDown = true;
		}

		if (keyDown) {
			m_velocity->set(0, 0);
			float distance = 1.0f;
			MathUtil::moveAngle<float>(m_velocity, angle, distance);
			m_lastAngle = angle;// + 90;
            m_buttonPushing = true;
		} else {
			m_buttonPushing = false;
		}

	Accelerometer* acc = Accelerometer::getInstance();
	if (container->isTouchMode() && acc->isEnabled()) {
		x = 0;
		y = 0;

		if (acc->acceleration()->y >= 0.15f) { // left
            x = -1.0f;
		} else if (acc->acceleration()->y <= -0.15f) { // right
			x = 1.0f;
		}
		if (acc->acceleration()->x >= 0.00f) { // up
			y = -1.0f;
		} else if (acc->acceleration()->x <= -0.3f) { // down
			y = 1.0f;
		}

		float angle = MathUtil::anglef(0, 0, x, y);
		//angle = fmod(angle, 45);

		if (m_snap) {
			signed int divisions = round(angle / 45.0f);
			angle = divisions * 45.0f;
		}

		float rawdistance = MathUtil::distance(0, 0, x, y);
		float distance = 1.0f;
		
		if (rawdistance > 0.25f) {
			m_velocity->set(0, 0);
			MathUtil::moveAngle<float>(m_velocity, angle, distance);
			m_lastAngle = angle;// + 90;
			m_stickPushing = true;
		} else {
			m_stickPushing = false;
		}
	}

	if (!m_buttonPushing && m_controllerInput && m_controllerIndex >= 0 && in->getGamepad(m_controllerIndex) != NULL) {
		// Gamepaaaaad!
		Gamepad* gamepad = in->getGamepad(m_controllerIndex);

		x = gamepad->getAxisValue(m_controllerAxisIndex); //axes.at(0)->value;
		y = gamepad->getAxisValue(m_controllerAxisIndex+1); //axes.at(1)->value;

		float angle = MathUtil::anglef(0, 0, x, y);
		//angle = fmod(angle, 45);

		if (m_snap) {
			signed int divisions = round(angle / 45.0f);
			angle = divisions * 45.0f;
		}
		//MathUtil::snap(45.0f, angle);



		//if (flippedControls) {
		//	angle += 180;
		//}

		float rawdistance = MathUtil::distance(0, 0, x, y);
		float distance = 1.0f; //rawSpeed * MathUtil::distance(0, 0, x, y);

		if (rawdistance > 0.25f) {
			m_velocity->set(0, 0);
			MathUtil::moveAngle<float>(m_velocity, angle, distance);
			m_lastAngle = angle;// + 90;
			m_stickPushing = true;
		} else {
			m_stickPushing = false;
		}

		bool keyDown = false;
		angle = 0;
		if (gamepad->isButtonDown(m_controllerUp) && gamepad->isButtonDown(m_controllerLeft)) {
			angle = -135;
			keyDown = true;
		} else if (gamepad->isButtonDown(m_controllerDown) && gamepad->isButtonDown(m_controllerLeft)) {
			angle = -225;
			keyDown = true;
		} else if (gamepad->isButtonDown(m_controllerUp) && gamepad->isButtonDown(m_controllerRight)) {
			angle = -45;
			keyDown = true;
		}  else if (gamepad->isButtonDown(m_controllerDown) && gamepad->isButtonDown(m_controllerRight)) {
			angle = -315;
			keyDown = true;
		} else if (gamepad->isButtonDown(m_controllerUp)) {
			angle = -90;
			keyDown = true;
		} else if (gamepad->isButtonDown(m_controllerDown)) {
			angle = 90;
			keyDown = true;
		} else if (gamepad->isButtonDown(m_controllerLeft)) {
			angle = 180;
			keyDown = true;
		} else if (gamepad->isButtonDown(m_controllerRight)) {
			angle = 0;
			keyDown = true;
		}

		if (keyDown) {
			m_velocity->set(0, 0);

			float distance = 1.0f;
			MathUtil::moveAngle<float>(m_velocity, angle, distance);
			m_lastAngle = angle;// + 90;
			m_buttonPushing = true;
		}
		else {
		 	m_buttonPushing = false;
		}


	}

}

EightWayMovementComponent::~EightWayMovementComponent() {
	 delete m_velocity;
	 m_velocity = NULL;
}
