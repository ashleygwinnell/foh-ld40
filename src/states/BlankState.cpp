#include "BlankState.h"

#include "../DefaultGame.h"

BlankState::BlankState():
	GameState() {

}

unsigned int BlankState::id() {
	return StatesList::STATE_BLANK;
}

void BlankState::enter(GameContainer* container, StateBasedGame* game, GameState* from) {

}

void BlankState::leave(GameContainer* container, StateBasedGame* game, GameState* to) {

}

void BlankState::init(GameContainer* container, StateBasedGame* game) {

}

void BlankState::update(GameContainer* container, StateBasedGame* game, GameTimer* timer) {

}

void BlankState::render(GameContainer* container, StateBasedGame* game, Renderer* r) {
	r->setDrawColor(Color::white);
	r->drawString("Blank State", 20, 20);
}

BlankState::~BlankState() {

}
