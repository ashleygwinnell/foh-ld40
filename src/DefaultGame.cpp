
#include "DefaultGame.h"
#include "objects/Player.h"






Image* ThreeSliceButton::s_buttonBorderLeft = NULL;
Image* ThreeSliceButton::s_buttonBorderMiddle = NULL;
Image* ThreeSliceButton::s_buttonBorderRight = NULL;

void ThreeSliceButton::init() {
	s_buttonBorderLeft = SpriteSheetStore::getImage("sprites/ui-button-3s-left.png");
	s_buttonBorderRight = SpriteSheetStore::getImage("sprites/ui-button-3s-right.png");
	s_buttonBorderMiddle = SpriteSheetStore::getImage("sprites/ui-button-3s-middle.png");
}

void ThreeSliceButton::draw(int x, int y, int width) {
	
    Renderer* r = ARK2D::getRenderer();
	s_buttonBorderLeft->drawAligned(x - (width/2), y, -1, 0);
	s_buttonBorderRight->drawAligned(x + (width/2), y, 1, 0);
	s_buttonBorderMiddle->drawCenteredScaled(x, y, width, 1.0f);
}


DefaultGame::DefaultGame(string title):
	StateBasedGame(title),
	skyColors(),
	stars(),
	trees(),
	layerxs(),
	m_explosions() {

}

void DefaultGame::initStates(GameContainer* container) {

	m_fmod = new ArkFMOD();
	m_fmod->init();

	stateBlank = new BlankState();
	stateSplash = new SplashState();
	stateMenu = new MenuState();
	stateInstructions = new InstructionsState();
	stateInGame = new InGameState();
	stateSummary = new SummaryState();

	Renderer::setInterpolation(Renderer::INTERPOLATION_NEAREST);
	spritesheet = Resource::get("all.png")->asImage();
	desc = Resource::get("all.spritesheetdescription")->asSpriteSheetDescription();
	SpriteSheetStore::addSheet(desc, spritesheet);

	ARK2D::getRenderer()->setInterpolation(Renderer::INTERPOLATION_NEAREST);
	font = Resource::get("fonts/SG12_small.fnt")->asFont()->asBMFont();
	font->setKerning(0);
	//font->scale(0.8f);
	ARK2D::getRenderer()->setDefaultFont(font);

	m_musicMenu = new ArkFMODSound(m_fmod, "music/menu.ogg");
	m_musicGame = new ArkFMODSound(m_fmod, "music/action.ogg");
	m_musicSummary = new ArkFMODSound(m_fmod, "music/gameover.ogg");
	m_musicCurrent = m_musicMenu;

	m_sfxMenuSelect = new ArkFMODSound(m_fmod, "sfx/menu-select.wav");
	m_sfxPlayerDie = new ArkFMODSound(m_fmod, "sfx/player-die.wav");
	m_sfxPlayerHurt = new ArkFMODSound(m_fmod, "sfx/player-hurt.wav");
	m_sfxPlayerJump = new ArkFMODSound(m_fmod, "sfx/player-jump.wav");
	m_sfxPlayerPickup = new ArkFMODSound(m_fmod, "sfx/boomerang-pickup.wav");
	m_sfxPlayerThrow = new ArkFMODSound(m_fmod, "sfx/boomerang-throw.wav");
	m_sfxPlayerThrow->setVolume(0.5f);
	m_sfxPresentPickup = new ArkFMODSound(m_fmod, "sfx/present-pickup.wav");
	m_sfxPuddingExplode = new ArkFMODSound(m_fmod, "sfx/bomb2.wav");
	m_sfxBomb = new ArkFMODSound(m_fmod, "sfx/bomb.wav");
	m_sfxBomb->setVolume(0.5f);
	m_sfxBombFuse = new ArkFMODSound(m_fmod, "sfx/bomb-fuse.wav");
	m_sfxMoleSpawn = new ArkFMODSound(m_fmod, "sfx/mole-spawn.wav");
	m_sfxMistletoeHurt = new ArkFMODSound(m_fmod, "sfx/mistletoe-hurt.wav");
	m_sfxMistletoeKiss = new ArkFMODSound(m_fmod, "sfx/mistletoe-kiss.wav");
	m_sfxMistletoeKiss->setVolume(0.4f);
	m_sfxSantaHurt = new ArkFMODSound(m_fmod, "sfx/santa-hurt-2.wav");
	m_sfxPowerup = new ArkFMODSound(m_fmod, "sfx/powerup.wav");
	m_sfxWin = new ArkFMODSound(m_fmod, "sfx/game-win.wav");

	m_explosions.push_back(m_sfxBomb);
	m_explosions.push_back(m_sfxPuddingExplode);
	m_explosions.push_back(m_sfxSantaHurt);
	m_explosions.push_back(m_sfxSantaHurt);

	m_reticule = SpriteSheetStore::getImage("sprites/reticule.png");

	

	m_floor = container->getHeight() - 24;
	ThreeSliceButton::init();
	m_buttonbg = SpriteSheetStore::getImage("sprites/ui-button.png");
	m_gradient = SpriteSheetStore::getImage("sprites/gradient.png");
	m_gradientWhite = SpriteSheetStore::getImage("sprites/gradient-white.png");
	m_treeLarge = SpriteSheetStore::getImage("sprites/tree-large.png");
	m_treeMedium = SpriteSheetStore::getImage("sprites/tree-medium.png");
	m_treeSmall = SpriteSheetStore::getImage("sprites/tree-small.png");

	addState(stateBlank);
	addState(stateSplash);
	addState(stateMenu);
	addState(stateInstructions);
	addState(stateInGame);
	addState(stateSummary);

	container->setSize(284*3, 160*3);

	skyColors.push_back(new Color("#740a66")); //closest the floor
	skyColors.push_back(new Color("#611768"));
	skyColors.push_back(new Color("#481768"));
	skyColors.push_back(new Color("#2f1353")); // sky

	layerxs.push_back(0.0f);
	layerxs.push_back(0.0f);
	layerxs.push_back(0.0f);

    //const Color p = const_cast<Color*>();
	ARK2D::getRenderer()->setScissorBoxColors(*skyColors[3]->constpointer(), Color::black, Color::black, Color::black);
	

	BGStar* star;
	star = new BGStar();
	star->x = 19; star->y = 11;
	star->animation = new Animation();
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-1.png")));
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-2.png")));
	star->animation->setFrameTime(1.0f);
	stars.push_back(star);

	star = new BGStar();
	star->x = 34; star->y = 24;
	star->animation = new Animation();
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-2.png")));
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-3.png")));
	star->animation->setFrameTime(1.5f);
	stars.push_back(star);

	star = new BGStar();
	star->x = 17; star->y = 38;
	star->animation = new Animation();
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-2.png")));
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-7.png")));
	star->animation->setFrameTime(1.5f);
	stars.push_back(star);

	star->x = 273; star->y = 42;
	star->animation = new Animation();
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-1.png")));
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-2.png")));
	star->animation->setFrameTime(1.0f);
	stars.push_back(star);

	star = new BGStar();
	star->x = 263; star->y = 62;
	star->animation = new Animation();
	star->animation->addFrame(spritesheet->getSubImage(desc->getItemByName("sprites/star-5.png")));
	star->animation->setFrameTime(0.66f);
	stars.push_back(star);

	float maxDuration = 16.0f;
	float numTrees = 7;
	for(unsigned int i = 0; i < numTrees; i++) { 
		BGTree* t = new BGTree();
		t->timer = (float) (maxDuration/numTrees) * i;
		t->duration = maxDuration;
		trees.push_back(t);
	}

	bgNightTimer = 0.0f;
	bgNightDuration = 20.0f;

	enterState(stateSplash);
}

void DefaultGame::update(GameContainer* container, GameTimer* timer) {
	StateBasedGame::update(container, timer);

	Input* i = ARK2D::getInput();
	if (i->isGamepadButtonPressed(Gamepad::BUTTON_ACTIVATE) || i->isKeyPressed(Input::KEY_F)) {
		container->setFullscreen(!container->isFullscreen());
	}

	if (i->isKeyPressed(Input::KEY_3)) {
		container->setSize(284*3, 160*3);
	}

	m_fmod->update(); 
	if (getCurrentState() == stateInGame) {
		if (m_musicMenu->isPlaying()) { m_musicMenu->stop(); }
		if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }

		if (!m_musicGame->isPlaying() && !stateInGame->m_win) {
			m_musicCurrent = m_musicGame;
			m_musicGame->play();
		}

		if (stateInGame->m_win) {
			if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
		}
	}
	else if (getCurrentState() == stateSummary) {
		if (m_musicMenu->isPlaying()) { m_musicMenu->stop(); }
		if (m_musicGame->isPlaying()) { m_musicGame->stop(); }

		if (!m_musicSummary->isPlaying()) {
			m_musicCurrent = m_musicSummary;

			if (!stateSummary->m_finalWin) {
				m_musicSummary->play();
				if (getCurrentState() == stateMenu) {
					m_musicSummary->setLowpass(true);
					m_musicSummary->setLowpassFrequency(500.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
				}
				else {
					m_musicSummary->setLowpass(true);
					m_musicSummary->setLowpassFrequency(22100.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
				}
			} else {
				if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }
				if (m_musicMenu->isPlaying()) {	m_musicMenu->stop(); }
				if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
			}
		}
	} 
	else if (getCurrentState() == stateMenu) {
		if (m_musicGame->isPlaying()) { m_musicGame->stop(); }
		if (m_musicSummary->isPlaying()) { m_musicSummary->stop(); }

		if (!m_musicMenu->isPlaying()) {
			m_musicCurrent = m_musicMenu;
			m_musicMenu->play();
			m_musicMenu->setLowpass(true);
			m_musicMenu->setLowpassFrequency(500.0f); //dg->m_musicGame->setLowpassFrequency(22100.0f);
		}
	}



	for(unsigned int i = 0; i < stars.size(); i++) {
		stars[i]->animation->update(timer);
	}
	for(unsigned int i = 0; i < trees.size(); i++) {
		trees[i]->timer += timer->getDelta();
		if (trees[i]->timer >= trees[i]->duration) {
			trees[i]->timer -= trees[i]->duration;
		}
	}

	m_reticuleRotation += timer->getDelta() * 135.0f;
	
	bgNightTimer += timer->getDelta();
	if (bgNightTimer >= bgNightDuration) {
		bgNightTimer -= bgNightDuration;
	}

	float mouseMultiplier = 0.0f;
	float mx = i->getMouseX();
	float hw = container->getWidth()*0.5f;
	if (getCurrentState() == stateInGame) {
		mx = stateInGame->m_player->m_bounds->getCenterX();
	}
	if (mx < hw) {
		float v = hw - mx;
		mouseMultiplier = Easing::ease(Easing::QUADRATIC_OUT, v, 0.0f, -1.0f, hw);
	} else if (mx > hw) {
		float v = mx - hw;
		mouseMultiplier = Easing::ease(Easing::QUADRATIC_OUT, v, 0.0f, 1.0f, hw);
	}
	layerxs[0] += timer->getDelta() * (stateInGame->m_santaHits * 1.0f * mouseMultiplier);
	layerxs[1] += timer->getDelta() * (stateInGame->m_santaHits * 2.0f * mouseMultiplier);
	layerxs[2] += timer->getDelta() * (stateInGame->m_santaHits * 3.0f * mouseMultiplier);

	if (layerxs[0] >= 4.0f) { layerxs[0] -= 4.0f; }
	if (layerxs[1] >= 4.0f) { layerxs[1] -= 4.0f; }
	if (layerxs[2] >= 4.0f) { layerxs[2] -= 4.0f; }

	if (layerxs[0] <= 4.0f) { layerxs[0] += 4.0f; }
	if (layerxs[1] <= 4.0f) { layerxs[1] += 4.0f; }
	if (layerxs[2] <= 4.0f) { layerxs[2] += 4.0f; }

}

void DefaultGame::render(GameContainer* container, Renderer* r) {
	StateBasedGame::render(container, r);
}

void DefaultGame::renderBackground() {
	Renderer* r = ARK2D::getRenderer();
    GameContainer* container = ARK2D::getContainer();
	r->setDrawColor(skyColors.at(skyColors.size()-1));
	r->fillRect(-20, -20, container->getWidth() + 40, container->getHeight() + 40);

	float nightHalf = bgNightDuration * 0.5f;
	float dayAlpha = 1.0f;
	/*if (bgNightTimer < nightHalf) {
		dayAlpha = Easing::easebetween(Easing::QUADRATIC_IN, bgNightTimer, 1.0f, 0.0f, nightHalf);
	} else {
		dayAlpha = Easing::easebetween(Easing::QUADRATIC_OUT, bgNightTimer - nightHalf, 0.0f, 1.0f, nightHalf);
	}*/

	// uint32_t side = Color::tweenHSV(currentSideColor, nextSideColor, t->anynumberFadeTimer / t->anynumberFadeDuration);
	r->setDrawColorf(1.0f, 1.0f, 1.0f, dayAlpha);
	float gradientBaseY = container->getHeight() - 24 - m_gradientWhite->getHeight() + 10;
	m_gradientWhite->setColor(skyColors.at(2));
		m_gradientWhite->draw(layerxs[2]+m_gradientWhite->getWidth()*-1.0f, gradientBaseY - 48);
		m_gradientWhite->draw(layerxs[2]+0.0f, gradientBaseY - 48);
		m_gradientWhite->draw(layerxs[2]+container->getWidth()+0.0f, gradientBaseY - 48);
	m_gradientWhite->setColor(skyColors.at(1));
		m_gradientWhite->draw(layerxs[1]+m_gradientWhite->getWidth()*-1.0f, gradientBaseY - 24);
		m_gradientWhite->draw(layerxs[1]+0.0f, gradientBaseY - 24);
		m_gradientWhite->draw(layerxs[1]+container->getWidth()+0.0f, gradientBaseY - 24);
	m_gradientWhite->setColor(skyColors.at(0));
		m_gradientWhite->draw(layerxs[0]+m_gradientWhite->getWidth()*-1.0f, gradientBaseY);
		m_gradientWhite->draw(layerxs[0]+0.0f, gradientBaseY);
		m_gradientWhite->draw(layerxs[0]+container->getWidth()+0.0f, gradientBaseY);

	r->setDrawColorf(1.0f, 1.0f, 1.0f, 1.0f);
	for(unsigned int i = 0; i < stars.size(); i++) {
		stars[i]->animation->getCurrentFrame()->drawCentered(stars[i]->x, stars[i]->y);
	}

	for (unsigned int i = 0; i < trees.size(); i++) {
		float treeX = Easing::easebetween(Easing::QUADRATIC_IN, trees[i]->timer, 62, -40, trees[i]->duration);
		float treeScale = Easing::easebetween(Easing::QUADRATIC_IN, trees[i]->timer, 0.0f, 1.5f, trees[i]->duration);
		m_treeLarge->drawAligned(treeX, m_floor, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, treeScale);
		m_treeLarge->drawAligned(container->getWidth() - treeX, m_floor, Renderer::ALIGN_CENTER, Renderer::ALIGN_BOTTOM, treeScale);
	}

	r->setDrawColor(Color::black);
	r->fillRect(-20, m_floor, container->getWidth()+40, 24 + 40);
}

void DefaultGame::render() {
	
	renderBackground();

	StateBasedGame::render();
	

	Renderer* r = ARK2D::getRenderer();

	r->setDrawColor(Color::white);
	Input* in = ARK2D::getInput();
	m_reticule->setRotation(m_reticuleRotation);
	m_reticule->drawCentered(in->getMouseX(), in->getMouseY());
}

void DefaultGame::resize(GameContainer* container, int width, int height) {
	StateBasedGame::resize(container, width, height);
}

DefaultGame* DefaultGame::getInstance() {
    return dynamic_cast<DefaultGame*>(ARK2D::getGame());
}

DefaultGame::~DefaultGame() {

}
