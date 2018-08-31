
#include "MenuState.h"
#include "../DefaultGame.h"

MenuState::MenuState():
	GameState() {

}

unsigned int MenuState::id() {
	return StatesList::STATE_MENU;
}

void MenuState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {
	m_outroTimer = 0.0f;
	m_introTimer = 0.01f;

	DefaultGame* dg = DefaultGame::getInstance();
	if (!dg->m_musicCurrent->isPlaying()) { 
		dg->m_musicCurrent->play(); 
	}
	if (dg->m_musicCurrent->isPlaying()) { 
		dg->m_musicCurrent->setLowpass(true); 
		dg->m_musicCurrent->setLowpassFrequency(500.0f);
	}
}

void MenuState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {
	DefaultGame* dg = DefaultGame::getInstance();
	if (dg->m_musicCurrent->isPlaying()) { 
		dg->m_musicCurrent->setLowpass(true); 
		dg->m_musicCurrent->setLowpassFrequency(22100.0f);
	}

}
void menuEvent_twitter() {
	SystemUtil::openBrowserToURL("http://twitter.com/forcehabit");
}
void menuEvent_play () {
    DefaultGame* dg = DefaultGame::getInstance();

    if (dg->stateMenu->m_introTimer > 0.0f || dg->stateMenu->m_outroTimer > 0.0f) { return; }

	dg->stateMenu->m_outroTo = dg->stateInGame;
	dg->stateMenu->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();

    
    //dg->stateInGame->reset();
    //dg->stateInGame->start();
}

void MenuState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();
	m_selectedIndex = 0;

	m_buttonPlay = new ARK::UI::Button();
	m_buttonPlay->setLocation(0, 0);
	m_buttonPlay->setSize(50, 24);
	m_buttonPlay->setEvent((void*) &menuEvent_play);
	m_buttonPlay->setLocationByCenter(container->getWidth()*0.5, container->getHeight()*0.7f);

	m_buttonFOH = new ARK::UI::Button();
	m_buttonFOH->setLocation(0, 0);
	m_buttonFOH->setSize(24, 24);
	m_buttonFOH->setEvent((void*) &menuEvent_twitter);
	m_buttonFOH->setLocationByCenter(container->getWidth() - 16, container->getHeight() - 16);


	twitter = SpriteSheetStore::getImage("sprites/ui-twitter.png");

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.5f;;
	m_outroTo = NULL;
}

void MenuState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	DefaultGame* dg = DefaultGame::getInstance();
	Input* i = ARK2D::getInput();

	if (!dg->m_musicCurrent->isPlaying()) { 
		dg->m_musicCurrent->play(); 
		dg->m_musicCurrent->setLowpass(true); 
	}
	dg->m_musicCurrent->setLowpassFrequency(500.0f);


	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
		}
	}
	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTimer > m_outroDuration) {
			if (m_outroTo == dg->stateInGame) {
				dg->stateInGame->reset();
				dg->stateInGame->start();
			}
			dg->enterState(m_outroTo);
			return;
		}
	}

	if (i->isKeyPressed(Input::KEY_R)) {
		m_introTimer = 0.01f;
	}

	if (i->isKeyPressed(Input::KEY_S)) {
		game->enterState(dg->stateSplash);
	}

	m_buttonPlay->update();
	m_buttonFOH->update();

	if (i->isKeyPressed(Input::KEY_ENTER) ||
		i->isGamepadButtonPressed(Gamepad::BUTTON_START) ||
		i->isGamepadButtonPressed(Gamepad::BUTTON_A)) {
		m_buttonPlay->doEvent();
		//if (m_selectedIndex == MENU_PLAY) {
			
			

		//}
		// else if (m_selectedIndex == MENU_INSTRUCTIONS) {
		// 	game->enterState(dg->stateInstructions);
		// }
		// else if (m_selectedIndex == MENU_EXIT) {
		// 	container->close();
		// }
	}
}

void MenuState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	DefaultGame* dg = DefaultGame::getInstance();
	
	float baseY = 0;
	float baseYBottom = 0;
	if (m_introTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
		baseYBottom = baseY;
	}
	else if (m_outroTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, container->getHeight() * -1.0f, m_outroDuration);
		baseYBottom = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, 50, m_outroDuration);
	}

	r->setDrawColor(Color::white);
	dg->font->drawString("CARL", container->getWidth()*0.5f, baseY + (container->getHeight()*0.2f), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 2.0f);
	dg->font->drawString("HATES", container->getWidth()*0.5f, baseY + (container->getHeight()*0.35f), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 2.0f);
	dg->font->drawString("CHRISTMAS", container->getWidth()*0.5f, baseY + (container->getHeight()*0.5f), Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 2.0f);
	
	ThreeSliceButton::draw(m_buttonPlay->transform.position.x, baseY+m_buttonPlay->transform.position.y, m_buttonPlay->getWidth());
	dg->font->drawString("PLAY", m_buttonPlay->transform.position.x - 1.5f, baseY+m_buttonPlay->transform.position.y + 2.0f, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.25f);

	ThreeSliceButton::draw(m_buttonFOH->transform.position.x, baseYBottom + m_buttonFOH->transform.position.y, m_buttonFOH->getWidth());
	twitter->drawCentered(m_buttonFOH->transform.position.x, baseYBottom + m_buttonFOH->transform.position.y);

	dg->font->drawString("(C) FORCE OF HABIT", container->getWidth()*0.5f, baseYBottom + container->getHeight()-10, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.0f);
}


bool MenuState::keyPressed(unsigned int key) {
	m_buttonPlay->keyPressed(key);
	m_buttonFOH->keyPressed(key);
	return false;
}
bool MenuState::keyReleased(unsigned int key) {
	m_buttonPlay->keyReleased(key);
	m_buttonFOH->keyReleased(key);
	return false;
}
bool MenuState::mouseMoved(int x, int y, int oldx, int oldy) {
	m_buttonPlay->mouseMoved(x, y, oldx, oldy);
	m_buttonFOH->mouseMoved(x, y, oldx, oldy);
	return false;
}

MenuState::~MenuState() {

}
