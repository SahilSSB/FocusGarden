#pragma once
#include <SFML/Graphics.hpp>
#include "World.h"
using namespace std;

enum GameState {
    ROAMING, 
    MENU,
    FOCUSSING
};

class Game {
    public:
        Game();
        void run();
    private:
        void processEvents();
        void update(Time dt);
        void render();

    sf::RenderWindow mWindow;
    GameState mState;
    World mWorld;

    bool mIsFocussing = false;
    float mFocusTimer = 0.f;
    const float TARGET_TIME = 0.166f * 60.f;

    sf::Font mFont;
    sf::Text mTimerText;
    sf::Text mStatusText;
};