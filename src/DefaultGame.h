
#ifndef ARK2D_DEFAULTGAME_H_
#define ARK2D_DEFAULTGAME_H_

#include <ARK2D/Audio/Sound.h>
#include <ARK2D/Core/Graphics/Image.h>
#include <ARK2D/Core/Graphics/SpriteSheetDescription.h>
#include <ARK2D/Core/State/StateBasedGame.h>
#include <ARK2D/Core/+.h>

#include "states/BlankState.h"
#include "states/InGameState.h"
#include "states/InstructionsState.h"
#include "states/MenuState.h"
#include "states/SplashState.h"
#include "states/SummaryState.h"

#include "util/FMOD.h" 

class StatesList {
	public:
		static const unsigned int STATE_BLANK = 0;
		static const unsigned int STATE_LOADING = 1;
		static const unsigned int STATE_SPLASH = 2;
		static const unsigned int STATE_MENU = 3;
		static const unsigned int STATE_INSTRUCTIONS = 4;
		static const unsigned int STATE_INGAME = 5;
		static const unsigned int STATE_SUMMARY = 6;
};

class BGTree {
	public:
		float timer;
		float duration;
};
class BGStar {
	public:
		float x;
		float y;
		Animation* animation;
};

class ThreeSliceButton {
	public:
		//static Image* s_buttonBackgroundLeft;
		//static Image* s_buttonBackgroundMiddle;
		//static Image* s_buttonBackgroundRight;
		static Image* s_buttonBorderLeft;
		static Image* s_buttonBorderMiddle;
		static Image* s_buttonBorderRight;
		static void init();
		static void draw(int x, int y, int width);
};



class DefaultGame : public StateBasedGame {
	public:

		BlankState* stateBlank = NULL;
		SplashState* stateSplash = NULL;
		MenuState* stateMenu = NULL;
		InstructionsState* stateInstructions = NULL;
		InGameState* stateInGame = NULL;
		SummaryState* stateSummary = NULL;

		Image* spritesheet = NULL;
		SpriteSheetDescription* desc = NULL;

		ARK::Core::Font::BMFont* font = NULL;

		//Sound* m_music = NULL;
		//Sound* m_sfxBomb = NULL;
		//Sound* m_sfxMenuSelect = NULL;

		ArkFMOD* m_fmod;
		ArkFMODSound* m_musicMenu;
		ArkFMODSound* m_musicGame;
		ArkFMODSound* m_musicSummary;
		ArkFMODSound* m_musicCurrent;
		//ArkFMODSound* m_sfxSelect;
		ArkFMODSound* m_sfxMenuSelect;
		ArkFMODSound* m_sfxPlayerDie;
		ArkFMODSound* m_sfxPlayerHurt;
		ArkFMODSound* m_sfxPlayerJump;
		ArkFMODSound* m_sfxPlayerThrow;
		ArkFMODSound* m_sfxPlayerPickup;
		ArkFMODSound* m_sfxPresentPickup;
		// ArkFMODSound* m_sfxButterflySmallDie;
		// ArkFMODSound* m_sfxPunchCharge;
		// ArkFMODSound* m_sfxPunchGo;
		ArkFMODSound* m_sfxBomb;
		ArkFMODSound* m_sfxBombFuse;
		ArkFMODSound* m_sfxMoleSpawn;
		ArkFMODSound* m_sfxPuddingExplode;
		ArkFMODSound* m_sfxMistletoeKiss;
		ArkFMODSound* m_sfxMistletoeHurt;
		ArkFMODSound* m_sfxSantaHurt;
		ArkFMODSound* m_sfxPowerup;
		ArkFMODSound* m_sfxWin;
		vector<ArkFMODSound*> m_explosions;
		// ArkFMODSound* m_sfxBombPlace;
		
		// ArkFMODSound* m_sfxTileAdvance;
		// ArkFMODSound* m_sfxArrowFire;
		// ArkFMODSound* m_sfxWaveUp;


		Image* m_reticule = NULL;
		float m_reticuleRotation = 0.0f;

		Image* m_buttonbg;
		Image* m_gradient;
		Image* m_gradientWhite;
		Image* m_treeLarge;
		Image* m_treeMedium;
		Image* m_treeSmall;
		float m_floor;
		vector<Color*> skyColors;
		vector<BGStar*> stars;
		vector<BGTree*> trees;
		vector<float> layerxs;

		float bgNightTimer;
		float bgNightDuration;

		DefaultGame(string title);
		virtual void initStates(GameContainer* container);
		virtual void update(GameContainer* container, GameTimer* timer);
		virtual void render(GameContainer* container, Renderer* r);
		virtual void render();
		virtual void renderBackground();
		virtual void resize(GameContainer* container, int width, int height);
		virtual ~DefaultGame();

		static DefaultGame* getInstance();
};

#endif /* ARK2D_DEFAULTGAME_H_ */
