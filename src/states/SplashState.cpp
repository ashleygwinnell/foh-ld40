
#include "SplashState.h"
#include "../DefaultGame.h"

SplashState::SplashState():
	GameState() {

}

unsigned int SplashState::id() {
	return StatesList::STATE_BLANK;
}

void SplashState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
	m_introTimer = 0.01f;
	m_waitTimer = 0.0f;
	m_outroTimer = 0.0f;
}

void SplashState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void SplashState::init(GameContainer* container, StateBasedGame* game) {
	logo = SpriteSheetStore::getImage("sprites/ui-foh.png");

	m_introTimer = 0.01f;
	m_introDuration = 0.4f;

	m_waitTimer = 0.0f;
	m_waitDuration = 0.7f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.4f;
}

void SplashState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {

	DefaultGame* dg = DefaultGame::getInstance();
	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
			m_waitTimer = 0.01f;
		}
	}

	if (m_waitTimer > 0.0f) {
		m_waitTimer += timer->getDelta();
		if (m_waitTimer > m_waitDuration) {
			m_waitTimer = 0.0f;
			m_outroTimer = 0.01f;
		}
	}

	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTimer >= m_outroDuration) {
			dg->m_sfxPuddingExplode->play();
		    dg->enterState(dg->stateMenu,
		        new FadeToColourTransition(1.0f, new Color(Color::white)),
		        new FadeFromColourTransition(1.0f, new Color(Color::white))
		    );

		}
	}
}

void SplashState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	r->setDrawColor(Color::white);

	float baseY = 0;
	if (m_introTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
	}
	else if (m_outroTimer > 0.0f) {
		//baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, -320, m_outroDuration);
	}

	r->setDrawColor("#2f1353",1.0f);
	r->fillRect(0,0,container->getWidth(),container->getHeight());

	r->setDrawColor(Color::white);
	logo->drawCentered(container->getWidth()*0.5f, baseY + (container->getHeight()*0.5f));

}

SplashState::~SplashState() {

}
