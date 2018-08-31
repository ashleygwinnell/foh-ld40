
#ifndef ARK2D_DEFAULTGAME_SUMMARYSTATE_H_
#define ARK2D_DEFAULTGAME_SUMMARYSTATE_H_

#include <ARK2D/Core/State/GameState.h>

class DefaultGame;

class MapLocation {
    public:
        Image* cityImage;
        float x;
        float y;

        float timer;
        float duration;
        bool ruined;
        static const unsigned int FACE_INTRO = 0;
        static const unsigned int FACE_IDLE = 1;
};

class SummaryState : public GameState {
	public:

		ARK::UI::Button* m_buttonRetry;
		ARK::UI::Button* m_buttonMenu;

		float m_introTimer;
		float m_introDuration;

		float m_outroTimer;
		float m_outroDuration;
		GameState* m_outroTo;

		float m_showUITimer;
		float m_showUIDuration;

		Image* m_map;
		Image* m_mapSadFace;
		Image* m_mapPresent;
		vector<MapLocation*> m_mapLocations;

		Image* m_santaSleigh;
		Image* m_santaHuman;
		Image* m_santaCarl;
		float m_santaTimer;
		float m_santaDuration;

		bool m_finalWin;
		int m_mapLocationsRuined;

		CameraShake* m_cameraShake;
		vector<string> winLines;
		vector<string> loseLines;
		int loseIndex;

		SummaryState();
		void enter(GameContainer* container, StateBasedGame* game, GameState* from);
		void leave(GameContainer* container, StateBasedGame* game, GameState* to);

		unsigned int id();
		float getSantaX();
		void init(GameContainer* container, StateBasedGame* game);
		void update(GameContainer* container, StateBasedGame* game, GameTimer* timer);
		void render(GameContainer* container, StateBasedGame* game, Renderer* r);

		virtual bool keyPressed(unsigned int key);
		virtual bool keyReleased(unsigned int key);
		virtual bool mouseMoved(int x, int y, int oldx, int oldy);

		virtual ~SummaryState();
};

#endif /* ARK2D_DEFAULTGAME_SUMMARYSTATE_H_ */
