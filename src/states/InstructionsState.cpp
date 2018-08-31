
#include "InstructionsState.h"
#include "../DefaultGame.h"

InstructionsState::InstructionsState():
	GameState() {

}

unsigned int InstructionsState::id() {
	return StatesList::STATE_INSTRUCTIONS;
}

void InstructionsState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {

}

void InstructionsState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void InstructionsState::init(GameContainer* container, StateBasedGame* game) {

}

void InstructionsState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {
	DefaultGame* dg = DefaultGame::getInstance();
	Input* i = ARK2D::getInput();

	if (i->isKeyPressed(Input::KEY_ENTER)) {
		game->enterState(dg->stateMenu);
	}
}

void InstructionsState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	r->setDrawColor(Color::white);
	r->drawString("Instructions State", 20, 20);
}

InstructionsState::~InstructionsState() {

}
