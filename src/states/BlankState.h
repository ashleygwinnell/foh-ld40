
#ifndef ARK2D_DEFAULTGAME_BLANKSTATE_H_
#define ARK2D_DEFAULTGAME_BLANKSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class BlankState : public GameState {

	public:
		BlankState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual ~BlankState();
};

#endif /* ARK2D_DEFAULTGAME_BLANKSTATE_H_ */
