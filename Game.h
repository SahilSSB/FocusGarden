#pragma once
#include <SFML/Graphics.hpp>
#include "World.h"
using namespace sf;
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
    RenderWindow mWindow;
    GameState mState;
    World mWorld;

    Time mFocusTimer;
};