
#include "SummaryState.h"
#include "../DefaultGame.h"

#include <ARK2D/Util/CameraShake.h>

SummaryState::SummaryState():
    GameState(),
    m_mapLocations(),
    loseLines(),
    loseIndex(0) {

}

unsigned int SummaryState::id() {
	return StatesList::STATE_SUMMARY;
}

void SummaryState::enter(GameContainer* container, StateBasedGame* game, GameState* from) { 
	m_outroTimer = 0.0f;
	m_introTimer = 0.01f;
	m_showUITimer = 0.0f;

	m_mapLocationsRuined = DefaultGame::getInstance()->stateInGame->m_score;
	m_finalWin = m_mapLocationsRuined >= m_mapLocations.size();
	m_santaDuration = 2.0f;

	loseIndex = MathUtil::randBetween(0, loseLines.size());

	for(unsigned int i = 0; i < m_mapLocations.size(); i++) {
		m_mapLocations[i]->timer = 0.0f; 
		m_mapLocations[i]->ruined = true;
	}

	DefaultGame* dg = DefaultGame::getInstance();
	if (!dg->m_musicCurrent->isPlaying()) { 
		dg->m_musicCurrent->play(); 
	}
	if (dg->m_musicCurrent->isPlaying()) {
		dg->m_musicCurrent->setLowpass(true); 
		dg->m_musicCurrent->setLowpassFrequency(500.0f);
	}

	if (m_finalWin) {
		dg->m_musicCurrent->stop();	
		dg->m_sfxWin->play();
		m_santaDuration = 7.0f;
	}
}
void SummaryState::leave(GameContainer* container, StateBasedGame* game, GameState* to) { 
}

void summaryEvent_retry() {
    DefaultGame* dg = DefaultGame::getInstance();
    if (dg->stateSummary->m_introTimer > 0.0f || dg->stateSummary->m_outroTimer > 0.0f || dg->stateSummary->m_showUITimer == 0.0f) { return; }

    dg->stateInGame->reset();
	dg->stateInGame->start();
    dg->stateSummary->m_outroTo = dg->stateInGame;
	dg->stateSummary->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();
}
void summaryEvent_menu() {
    DefaultGame* dg = DefaultGame::getInstance();
    if (dg->stateSummary->m_introTimer > 0.0f || dg->stateSummary->m_outroTimer > 0.0f || dg->stateSummary->m_showUITimer == 0.0f) { return; }

    dg->stateSummary->m_outroTo = dg->stateMenu;
	dg->stateSummary->m_outroTimer = 0.01f;

    dg->m_sfxMenuSelect->play();
}
void SummaryState::init(GameContainer* container, StateBasedGame* game) {
	DefaultGame* dg = DefaultGame::getInstance();

	m_buttonMenu = new ARK::UI::Button();
	m_buttonMenu->setLocation(0, 0);
	m_buttonMenu->setSize(55, 24);
	m_buttonMenu->setEvent((void*) &summaryEvent_menu);
	m_buttonMenu->setLocationByCenter(110, container->getHeight() - 20);

	m_buttonRetry = new ARK::UI::Button();
	m_buttonRetry->setLocation(0, 0);
	m_buttonRetry->setSize(60, 24);
	m_buttonRetry->setEvent((void*) &summaryEvent_retry);
	m_buttonRetry->setLocationByCenter(container->getWidth() - 110, container->getHeight() - 20);

	m_santaSleigh = SpriteSheetStore::getImage("sprites/santa-sleigh.png");
	m_santaHuman = SpriteSheetStore::getImage("sprites/santa-idle-1.png");
	m_santaCarl = SpriteSheetStore::getImage("sprites/carl-idle-1.png");
	m_santaTimer = 0.0f;
	m_santaDuration = 2.0f;

	m_showUITimer = 0.0f;
	m_showUIDuration = 0.5f;;


	loseLines.push_back("SANTA PREVAILS!");
	loseLines.push_back("YOUR PARENTS MISS YOU!");
	loseLines.push_back("ICE COLD DEATH");
	loseLines.push_back("GRANDMA KISSED YOU");
	loseLines.push_back("EW, RAISINS!");
	loseLines.push_back("CHRISTMAS GOES ON!");
	loseLines.push_back("SING-ALONG SONGS!");
	loseLines.push_back("GAME OVER");

	m_map = SpriteSheetStore::getImage("sprites/earth-blackborder.png");
	m_mapSadFace = SpriteSheetStore::getImage("sprites/earth-sadface.png");
	m_mapPresent = SpriteSheetStore::getImage("sprites/present-1.png");

	// add places.
	auto addLocation = [this](float x, float y, Color* c, string spr) -> void {
		MapLocation* ml = new MapLocation();
		ml->x = x;
		ml->y = y;
		ml->cityImage = SpriteSheetStore::getImage(spr);
        ml->cityImage->setColor(c);
        ml->timer = 0.0f;
        ml->duration = 1.0f;
		m_mapLocations.push_back(ml);
    };

	addLocation(35,68, Color::white.pointer(), "sprites/earth-city-building.png");
	addLocation(54,57, Color::white.pointer(), "sprites/earth-city-building.png");
	addLocation(62,85, Color::white.pointer(), "sprites/earth-city-building.png");
	addLocation(77,74, Color::white.pointer(), "sprites/earth-city-building.png");

	addLocation(129,50, Color::black.pointer(), "sprites/earth-city-igloo.png");
	
	addLocation(135,103, Color::black.pointer(), "sprites/earth-city-pyramid.png");
	addLocation(169,118, Color::black.pointer(), "sprites/earth-city-pyramid.png");
	addLocation(181,114, Color::black.pointer(), "sprites/earth-city-pyramid.png");

	addLocation(186,84, Color::white.pointer(), "sprites/earth-city-building.png");
	addLocation(201,73, Color::white.pointer(), "sprites/earth-city-building.png");

	addLocation(230,82, Color::white.pointer(), "sprites/earth-city-temple.png");
	addLocation(241,94, Color::white.pointer(), "sprites/earth-city-temple.png");

	m_introTimer = 0.01f;
	m_introDuration = 0.5f;

	m_outroTimer = 0.0f;
	m_outroDuration = 0.5f;;
	m_outroTo = NULL;

	m_finalWin = false; 
	m_mapLocationsRuined = 5;

	m_cameraShake = new CameraShake(game, 3);
}

void SummaryState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	
	DefaultGame* dg = DefaultGame::getInstance();
	Input* in = ARK2D::getInput();
	if (in->isKeyPressed(Input::KEY_ENTER) ||
        in->isGamepadButtonPressed(Gamepad::BUTTON_START) ||
        in->isGamepadButtonPressed(Gamepad::BUTTON_A)) {
		//dg->enterState(dg->stateMenu);
		m_buttonMenu->doEvent();
	}

	if (!m_finalWin) {
		if (!dg->m_musicCurrent->isPlaying()) { 
			dg->m_musicCurrent->play(); 
			dg->m_musicCurrent->setLowpass(true); 
		}
		dg->m_musicCurrent->setLowpassFrequency(500.0f);
	}

	if (m_introTimer > 0.0f) {
		m_introTimer += timer->getDelta();
		if (m_introTimer > m_introDuration) {
			m_introTimer = 0.0f;
			m_santaTimer = 0.01f;
		}
	}
	if (m_outroTimer > 0.0f) {
		m_outroTimer += timer->getDelta();
		if (m_outroTimer > m_outroDuration) {
			dg->enterState(m_outroTo);
			return;
		}
	}

	if (m_santaTimer > 0.0f) {
		m_santaTimer += timer->getDelta();
		if (m_santaTimer >= m_santaDuration) {
			m_santaTimer = 0.0f;

			m_showUITimer = 0.01f;
		}

		float eachTime = m_santaDuration / float(m_mapLocations.size());
		for(unsigned int i = 0; i < m_mapLocations.size(); i++) {
			//if (m_santaTimer > float(i) * eachTime && m_mapLocations[i]->timer == 0.0f) {
			if (getSantaX() > m_mapLocations[i]->x && m_mapLocations[i]->timer == 0.0f) {
				//getSantaX();
				m_mapLocations[i]->timer = 0.01f;
				m_mapLocations[i]->ruined = (m_mapLocationsRuined > i);
				if (m_mapLocations[i]->ruined) {
					m_cameraShake->setSpeedMultiplier(1, 0.5f);
					m_cameraShake->start(1, 1.0f);

					//if (m_finalWin) {
						dg->m_explosions[MathUtil::randBetween(0, dg->m_explosions.size())]->play();
					//}

				} else {
					DefaultGame::getInstance()->m_sfxPresentPickup->play();
				}


			}
		}
	}

	if (m_showUITimer > 0.0f) {
		m_showUITimer += timer->getDelta();
		if (m_showUITimer >= m_showUIDuration) {
			m_showUITimer = m_showUIDuration;
		}
	}

	for(unsigned int i = 0; i < m_mapLocations.size(); i++) {
		if (m_mapLocations[i]->timer > 0.0f) {
			m_mapLocations[i]->timer += timer->getDelta();
		}
	}



	if (in->isKeyPressed(Input::KEY_R)) {
		m_introTimer = 0.01f;

		for(unsigned int i = 0; i < m_mapLocations.size(); i++) {
			m_mapLocations[i]->timer = 0.0f; 
			m_mapLocations[i]->ruined = false;
		}
	}

	m_cameraShake->update(container, timer);

	m_buttonRetry->update();
	m_buttonMenu->update();


}  

float SummaryState::getSantaX() {
    return Easing::easebetween(Easing::QUADRATIC_IN_OUT, m_santaTimer, -20, ARK2D::getContainer()->getWidth()+40, m_santaDuration);
}

void SummaryState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {

	DefaultGame* dg = DefaultGame::getInstance();

	float buttonsUIY = 50;
	if (m_showUITimer > 0.0f) {
		buttonsUIY = Easing::easebetween(Easing::QUADRATIC_OUT, m_showUITimer, 50, 0, m_showUIDuration);
	}

	float baseY = 0;
	float mult = 1.0f;
	if (m_introTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, container->getHeight(), 0, m_introDuration);
		mult = Easing::easebetween(Easing::QUADRATIC_OUT, m_introTimer, 0.0f, 1.0f, m_introDuration);
	}
	else if (m_outroTimer > 0.0f) {
		baseY = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 0, container->getHeight() * -1.0f, m_outroDuration);
		mult = Easing::easebetween(Easing::QUADRATIC_OUT, m_outroTimer, 1.0f, 0.0f, m_outroDuration);
	}



	r->setDrawColor(dg->skyColors[dg->skyColors.size()-1], mult);
	r->fillRect(0,0,container->getWidth(), container->getHeight());


	float endScale = 2.0f;
	string endString = (m_finalWin)?"REJOICE! XMAS IS CANCELLED!":loseLines[loseIndex];
	float endWidth = dg->font->getStringWidth(endString) * endScale;
	if (endWidth > container->getWidth() - 40) {
		endScale *= (container->getWidth() - 40) / endWidth;
	}

	r->pushMatrix();
	r->translate(m_cameraShake->getXOffset(), m_cameraShake->getYOffset());

	r->setDrawColor(Color::white);
	if (m_showUITimer > 0.0f) {
		dg->font->drawString(endString, (container->getWidth()*0.5f)- 2, 25 + baseY - buttonsUIY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, endScale);
	}

	m_map->drawCentered(container->getWidth()*0.5f, (container->getHeight()*0.5f) + baseY);
	for(unsigned int i = 0; i < m_mapLocations.size(); i++) {
		m_mapLocations[i]->cityImage->drawAligned(m_mapLocations[i]->x, m_mapLocations[i]->y + baseY, 0, 1, 1.0f);

		float sc = Easing::easebetween(Easing::QUADRATIC_OUT, m_mapLocations[i]->timer, 0.0f, 1.0f, m_mapLocations[i]->duration);
		Image* img = (!m_mapLocations[i]->ruined)?m_mapPresent:m_mapSadFace;
		img->drawAligned(m_mapLocations[i]->x, m_mapLocations[i]->y + baseY - m_mapLocations[i]->cityImage->getHeight() - 1, 0, 1, sc);
	}

	float santaY = 25.0;
	m_santaSleigh->drawCentered(getSantaX(), santaY);
	if (!m_finalWin) { 
		m_santaHuman->drawCentered(getSantaX() + 4, santaY - 5);
	} else {
		m_santaCarl->drawCentered(getSantaX() + 2, santaY - 4);
	}

	r->popMatrix();

	//m_buttonRetry

	if (m_finalWin) {
		m_buttonMenu->setLocationByCenter(container->getWidth() * 0.5, container->getHeight() - 20);
		m_buttonRetry->setLocationByCenter(container->getWidth() * 0.5, container->getHeight() + 1000);

		ThreeSliceButton::draw(m_buttonMenu->transform.position.x, baseY+m_buttonMenu->transform.position.y+ buttonsUIY, m_buttonMenu->getWidth());
		dg->font->drawString("FIN", m_buttonMenu->transform.position.x - 1.5f, baseY+m_buttonMenu->transform.position.y + 2.0f + buttonsUIY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.25f);	
	} else {
		m_buttonMenu->setLocationByCenter(110, container->getHeight() - 20);
		m_buttonRetry->setLocationByCenter(container->getWidth() - 110, container->getHeight() - 20);

		ThreeSliceButton::draw(m_buttonRetry->transform.position.x, baseY+m_buttonRetry->transform.position.y+ buttonsUIY, m_buttonRetry->getWidth());
		dg->font->drawString("RETRY", m_buttonRetry->transform.position.x - 1.5f, baseY+m_buttonRetry->transform.position.y + 2.0f + buttonsUIY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.25f);

		ThreeSliceButton::draw(m_buttonMenu->transform.position.x, baseY+m_buttonMenu->transform.position.y+ buttonsUIY, m_buttonMenu->getWidth());
		dg->font->drawString("MENU", m_buttonMenu->transform.position.x - 1.5f, baseY+m_buttonMenu->transform.position.y + 2.0f + buttonsUIY, Renderer::ALIGN_CENTER, Renderer::ALIGN_CENTER, 0.0f, 1.25f);	
	}

	

}

bool SummaryState::keyPressed(unsigned int key) {
	m_buttonRetry->keyPressed(key);
	m_buttonMenu->keyPressed(key);
	return false;
}
bool SummaryState::keyReleased(unsigned int key) {
	m_buttonRetry->keyReleased(key);
	m_buttonMenu->keyReleased(key);
	return false;
}
bool SummaryState::mouseMoved(int x, int y, int oldx, int oldy) {
	m_buttonRetry->mouseMoved(x, y, oldx, oldy);
	m_buttonMenu->mouseMoved(x, y, oldx, oldy);
	return false;
}

SummaryState::~SummaryState() {

}

