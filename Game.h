#pragma once
#include <SFML/Graphics.hpp>
#include "World.h"
#include "GameState.h"
using namespace std;

class Game {
    public:
        Game();
        void run();
    private:
        void processEvents();
        void update(sf::Time dt);
        void render();
        void handlePlayerInput(sf::Keyboard::Scancode code, bool isPressed);

    sf::RenderWindow mWindow;
    GameState mState;
    World mWorld;
    sf::View mWorldView;
    sf::View mUIView;

    bool mIsFocussing = false;
    float mFocusTimer = 0.f;
    const float TARGET_TIME = 0.166f * 60.f;

    bool mShowDoorPrompt;
    bool mShowExitPrompt;
    sf::RectangleShape mPromptBox;
    sf::Text mPromptText;
    
    sf::Font mFont;
    sf::Text mTimerText;
    sf::Text mStatusText;

    bool mIsPaused = false;
    sf::RectangleShape mMenuBackrgound;
    sf::Text mResumeText;
    sf::Text mQuitText;

    bool mShowComputerUI;
    sf::RectangleShape mMonitorFrame;
    sf::RectangleShape mMonitorScreen;
    sf::Text mComputerText;

    PomoState mPomoState;
    int mSessionsCompeleted;
};