#pragma once
#include <SFML/Graphics.hpp>
#include "World.h"
#include "GameState.h"
#include "SessionManager.h"
using namespace std;

class Game {
    public:
        Game();
        void run();
    private:
        void processEvents();
        void update(sf::Time dt);
        void render();
        void handleRoamingInput(const sf::Event& event);
        void handleComputerInput(const sf::Event& event);
        void handleWarningInput(const sf::Event& event);
        void handlePauseMenuInput(const sf::Event& event);
        void handleWindowResize(const sf::Event::Resized& resizeed);


        sf::RenderWindow mWindow;
        GameState mState;
        World mWorld;
        sf::View mWorldView;
        sf::View mUIView;

        SessionManager mSessionManager;

        float mConfigurableTimer = 25.f * 60.f;

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

        float mTimeSinceInput;
        static constexpr float IDLE_THRESHOLD = 5.f;

        sf::Text mEditModeText;
        bool mIsEditing = false;

        bool mShowQuitWarning;
        sf::RectangleShape mWarningBox;
        sf::Text mWarningText;
        sf::Text mWarningYesText;
        sf::Text mWarningNoText;

        void setupCallbacks();
        void updateComputerUIText();
};