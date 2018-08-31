
#ifndef ARK2D_DEFAULTGAME_INSTRUCTIONSSTATE_H_
#define ARK2D_DEFAULTGAME_INSTRUCTIONSSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class InstructionsState : public GameState {
	public:
		InstructionsState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual ~InstructionsState();
};

#endif /* ARK2D_DEFAULTGAME_INSTRUCTIONSSTATE_H_ */
