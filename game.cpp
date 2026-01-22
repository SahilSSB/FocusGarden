#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "World.h"
#include "Interior.h"
using namespace std;
using namespace sf;

Game::Game() : mWindow(VideoMode({800,600}), "Focus Garden"),
        mState(GameState::ROAMING),
        mTimerText(mFont),
        mStatusText(mFont),
        mResumeText(mFont),
        mQuitText(mFont),
        mPromptText(mFont),
        mComputerText(mFont)
{
    mWindow.setFramerateLimit(60);
    ContextSettings settings;
    settings.antiAliasingLevel = 0;
    mIsPaused = false;
    mMenuBackrgound.setSize(Vector2f({800,600}));
    mMenuBackrgound.setFillColor(Color(0, 0, 0, 150));

    mResumeText.setString("Resume");
    mResumeText.setCharacterSize(40);
    mResumeText.setFillColor(Color::White);
    mResumeText.setOrigin({mResumeText.getLocalBounds().size.x / 2.f,
                        mResumeText.getLocalBounds().size.y / 2.f});
    mResumeText.setPosition({300, 220});

    mQuitText.setFont(mFont);
    mQuitText.setString("QUIT & SAVE");
    mQuitText.setCharacterSize(40);
    mQuitText.setFillColor(Color::White);
    mQuitText.setOrigin({mQuitText.getLocalBounds().size.x / 2.f, 
                        mQuitText.getLocalBounds().size.y / 2.f});
    mQuitText.setPosition({300, 320}); 

    try {
        mWorld.init();
        mWorld.initInterior();

        mWorld.load("garden.dat");

        mWorld.getPlayer().setCollissionCallback([this](Vector2f pos) {
            return mWorld.isPositionBlocked(pos, mState);
        });

        if (!mFont.openFromFile("fonts/JetBrainsMonoNLNerdFontMono-Regular.ttf")) {
            cerr << "WARNING: font not found" << endl;
        }

        mPromptBox.setSize({400.f, 120.f});
        mPromptBox.setFillColor(sf::Color(0, 0, 0, 220));
        mPromptBox.setOutlineColor(sf::Color::White);
        mPromptBox.setOutlineThickness(2.f);
        mPromptBox.setOrigin({200.f, 60.f});
        mPromptBox.setPosition({400.f, 500.f});

        mPromptText.setFont(mFont);
        mPromptText.setCharacterSize(24);
        mPromptText.setFillColor(sf::Color::White);

        mShowDoorPrompt = false;
        mShowExitPrompt = false;

        mShowComputerUI = false;

        mMonitorFrame.setSize({600.f, 400.f});
        mMonitorFrame.setFillColor(sf::Color(200, 200, 180));
        mMonitorFrame.setOutlineColor(sf::Color(50, 50, 50));
        mMonitorFrame.setOutlineThickness(4.f);
        mMonitorFrame.setOrigin({300.f, 200.f});
        mMonitorFrame.setPosition({400.f, 300.f});

        mMonitorScreen.setSize({560.f, 320.f});
        mMonitorScreen.setFillColor(sf::Color(20, 20, 40));
        mMonitorScreen.setOrigin({280.f, 160.f});
        mMonitorScreen.setPosition({400.f, 300.f});

        mComputerText.setFont(mFont);
        mComputerText.setCharacterSize(18);
        mComputerText.setFillColor(sf::Color(0, 255, 0));
        mComputerText.setString("SYSTEM READY...\n\n[SPACE] Start Session\n[ESC] Log Out");
        mComputerText.setPosition({140.f, 160.f});
                
        Vector2u winSize = mWindow.getSize();
        float w = static_cast<float>(winSize.x);
        float h = static_cast<float>(winSize.y);
        FloatRect mapBounds = mWorld.getBounds();
        float mapHeight = mapBounds.size.y;
        
        float desiredZoom = mapHeight / (w / 2.f);
        if (desiredZoom <= 0.f) desiredZoom = 1.0f;
        
        mWorldView = mWindow.getDefaultView(); 
        mWorldView.setSize({w * desiredZoom, h * desiredZoom});
        mWorldView.setCenter(mapBounds.getCenter());

        mUIView = mWindow.getDefaultView();
    }
    catch (const exception& e) {
    cerr << "Error: " << e.what() << endl;
    mWindow.close();
    }
}

void Game::run() {
    Clock clock;
    Time timeSinceLastUpdate = Time::Zero;

    while (mWindow.isOpen()) {
        processEvents();
        Time dt = clock.restart();
        update(dt);

        render();
    }
}

void Game::processEvents() {
    while (const auto event = mWindow.pollEvent()) {
        if (event->is<Event::Closed>()) {
            mWorld.save("garden.dat");
            mWindow.close();
        }

        if (mShowComputerUI) {
            if (const auto* keyPress = event->getIf<Event::KeyPressed>()) {

                if (keyPress->scancode == Keyboard::Scancode::Escape) {
                    mShowComputerUI = false;
                    Vector2u winSize = mWindow.getSize();
                    float w = static_cast<float>(winSize.x);
                    float h = static_cast<float>(winSize.y);
                    FloatRect roomBounds = mWorld.getInterior().getBounds();
                    float desiredZoom = roomBounds.size.y / (w / 2.f);
                    if (desiredZoom < 0.1f) desiredZoom = 1.f;
                    mWorldView.setSize({w * desiredZoom, h * desiredZoom});
                }
                else if (keyPress->scancode == Keyboard::Scancode::Space) {
                    mIsFocussing = !mIsFocussing;
                    if (mIsFocussing) {
                        if (mFocusTimer <= 0.f) mFocusTimer = TARGET_TIME;
                    }
                }
            }
            continue;
        }
        if (mShowDoorPrompt) {
            if (const auto* keyPress = event->getIf<Event::KeyPressed>()) {
                if (keyPress->scancode == Keyboard::Scancode::Y ||
                    keyPress->scancode == Keyboard::Scancode::Enter) {
                        
                        mShowDoorPrompt = false;
                        mState = GameState::INSIDE_HOUSE;
                        mWorld.setPlayerPosition(mWorld.getInterior().IgridToIso(10, 19));
                        mWorld.disablePlayerCollision();
                        Vector2u winSize = mWindow.getSize();
                        float w = static_cast<float>(winSize.x);
                        float h = static_cast<float>(winSize.y);
                        FloatRect roomBounds = mWorld.getInterior().getBounds();
                        float desiredZoom = roomBounds.size.y / (w / 2.f);
                        if (desiredZoom < 0.1f) desiredZoom = 1.f; 
                        mWorldView.setSize({w * desiredZoom, h * desiredZoom});
                        cout << "Entered house" << endl;
                    }
                else if (keyPress->scancode == Keyboard::Scancode::N ||
                         keyPress->scancode == Keyboard::Scancode::Escape) {

                            mShowDoorPrompt = false;
                            Vector2f currentPos = mWorld.getPlayerPosition();
                            mWorld.setPlayerPosition({currentPos.x, currentPos.y + 15.f});
                        }
            }
            continue;
        }

        if (mShowExitPrompt) {
            if (const auto* keyPress = event->getIf<Event::KeyPressed>()) {
            if (keyPress->scancode == Keyboard::Scancode::Y ||
                keyPress->scancode == Keyboard::Scancode::Enter) {
                
                mShowExitPrompt = false;
                mState = GameState::ROAMING;
                mWorld.setPlayerPosition(mWorld.gridToIso(9, 8));
                Vector2u winSize = mWindow.getSize();
                float w = static_cast<float>(winSize.x);
                float h = static_cast<float>(winSize.y);
                FloatRect gardenBounds = mWorld.getBounds();
                float desiredZoom = gardenBounds.size.y / (w / 2.f);
                if (desiredZoom <= 0.f) desiredZoom = 1.0f;
                mWorldView.setSize({w * desiredZoom, h * desiredZoom});
                mWorldView.setCenter(gardenBounds.getCenter());
                mWorld.enablePlayerCollision();
                cout << "Left the house" << endl;
            }
            else if (keyPress->scancode == Keyboard::Scancode::N ||
                    keyPress->scancode == Keyboard::Scancode::Escape) {
                
                    mShowExitPrompt = false;
                    Vector2f currentPos = mWorld.getPlayerPosition();
                    mWorld.setPlayerPosition({currentPos.x, currentPos.y - 15.f});
                }
            }
            continue;
        }
        else if (const auto* resized = event->getIf<Event::Resized>()) {
            float w = static_cast<float>(resized->size.x);
            float h = static_cast<float>(resized->size.y);
            
            mUIView.setSize({w, h});
            mUIView.setCenter({w / 2.f, h / 2.f});
            FloatRect currentMapBounds;
            if (mState == GameState::INSIDE_HOUSE) {
                currentMapBounds = mWorld.getInterior().getBounds();
            }
            else {
                currentMapBounds = mWorld.getBounds();
            }
            float mapHeight = currentMapBounds.size.y;
            float desiredZoom = mapHeight / (w / 2.f);
            mWorldView.setSize({w * desiredZoom, h * desiredZoom});
            mWorldView.setCenter(currentMapBounds.getCenter());
        }
        else if (const auto* mousePress = event->getIf<Event::MouseButtonPressed>()) {
            if (mousePress->button == Mouse::Button::Left) {
                if (!mIsFocussing) {
                    Vector2i pixelPos = mousePress->position;
                    Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos, mWorldView);
                    Vector2i gridPos = mWorld.isoToGrid(static_cast<float>(worldPos.x),
                                                        static_cast<float>(worldPos.y));
                    mWorld.toggleTree(gridPos.x, gridPos.y);
                }
                if (mIsPaused) {
                    Vector2i mousePos = Mouse::getPosition(mWindow);
                    Vector2f worldPos = mWindow.mapPixelToCoords(mousePos, mWindow.getDefaultView());
                    
                    if (mResumeText.getGlobalBounds().contains(worldPos)) {
                        mIsPaused = false;
                    }
                    else if (mQuitText.getGlobalBounds().contains(worldPos)) {
                        mWorld.save("garden.dat");
                        mWindow.close();
                    }
                else {
                    Vector2i mousePos = Mouse::getPosition(mWindow);
                    Vector2f worldPos = mWindow.mapPixelToCoords(mousePos, mWorldView);
                    Vector2i gridPos = mWorld.isoToGrid(worldPos.x, worldPos.y);
                    mWorld.toggleTree(gridPos.x, gridPos.y);
                    }
                }
            }
        }
        else if (const auto* mouseMove = event->getIf<Event::MouseMoved>()) {
            if (!mIsFocussing) {
                Vector2i pixelPos = mouseMove->position;
                Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos, mWorldView);
                Vector2i gridPos = mWorld.isoToGrid(worldPos.x, worldPos.y);

                mWorld.setHoveredTile(gridPos);
            }
            else {
                mWorld.setHoveredTile({-1, -1});
            }
            
        }
        else if (const auto* keyPress = event->getIf<Event::KeyPressed>()) {
            if (keyPress->scancode == Keyboard::Scancode::Enter) {
                if (!mIsFocussing && !mIsPaused) {
                    if (mState == GameState::INSIDE_HOUSE &&
                        mWorld.getInterior().isComputer(mWorld.getPlayerPosition())) {
                            mShowComputerUI = true;
                        }
                    else {
                        mWorld.interact();
                    }
                }
            }
            else if (keyPress->scancode == Keyboard::Scancode::Escape) {
                mIsPaused = !mIsPaused;
            }
        }
    }
}

void Game::update(Time dt) {
    if (mIsPaused) {
        return;
    }
    if (mShowDoorPrompt || mShowExitPrompt) {
        return;
    }
    if (mIsFocussing) {
        mFocusTimer -= dt.asSeconds();
        if (mFocusTimer <= 0.f) {
            mFocusTimer = 0.f;
            mIsFocussing = false;
            mStatusText.setString("SESSION COMPLETE!");
            mWorld.finishSession();
        }
    }
    int minutes = static_cast<int>(mFocusTimer) / 60;
    int seconds = static_cast<int>(mFocusTimer) % 60;
    string timeStr = to_string(minutes) + ":";
    if (seconds < 10) timeStr += "0";
    timeStr += to_string(seconds);
    mTimerText.setString(timeStr);
    
    mWorld.update(dt, mIsFocussing);

    Vector2f targetPos = mWorld.getPlayerPosition();

    if (mState == GameState::ROAMING) {
        if (mWorld.checkDoorEntry(mWorld.getPlayerPosition())) {
            mShowDoorPrompt = true;
            return;
        }
        if (mWorld.checkDoorEntry(mWorld.getPlayerPosition())) {
            mState = GameState::INSIDE_HOUSE;
            mWorld.setPlayerPosition(mWorld.getInterior().IgridToIso(10, 19));
            mWorld.disablePlayerCollision();
            mWorldView.setSize({800.f, 600.f});
            mWorldView.setCenter(mWorld.getPlayerPosition());
            cout << "Entered house" << endl;
            return;
        }
    }
    else if (mState == GameState::INSIDE_HOUSE) {
        if (mWorld.getInterior().isExit(mWorld.getPlayerPosition())) {
            mShowExitPrompt = true;
            return;
        }
        FloatRect mapBounds = mWorld.getInterior().getBounds();
        Vector2f viewSize = mWorldView.getSize();

        float minX = mapBounds.position.x + viewSize.x / 2.f;
        float maxX = mapBounds.position.x + mapBounds.size.x - viewSize.x / 2.f;
        float minY = mapBounds.position.y + viewSize.y / 2.f;
        float maxY = mapBounds.position.y + mapBounds.size.y - viewSize.y / 2.f;

        if (targetPos.x < minX) targetPos.x = minX;
        if (targetPos.x > maxX) targetPos.x = maxX;
        if (targetPos.y < minY) targetPos.y = minY;
        if (targetPos.y > maxY) targetPos.y = maxY;

        if (minX > maxX) targetPos.x = mapBounds.position.x + mapBounds.size.x / 2.f;
        if (minY > maxY) targetPos.y = mapBounds.position.y + mapBounds.size.y / 2.f;

        mWorldView.setCenter(targetPos);
    }
    if (mState == GameState::ROAMING) {
        FloatRect mapBounds = mWorld.getBounds();
        Vector2f viewSize = mWorldView.getSize();

        float minX = mapBounds.position.x + viewSize.x / 2.f;
        float maxX = mapBounds.position.x + mapBounds.size.x - viewSize.x / 2.f;
        float minY = mapBounds.position.y + viewSize.y / 2.f;
        float maxY = mapBounds.position.y + mapBounds.size.y - viewSize.y / 2.f;

        if (targetPos.x < minX) targetPos.x = minX;
        if (targetPos.x > maxX) targetPos.x = maxX;
        if (targetPos.y < minY) targetPos.y = minY;
        if (targetPos.y > maxY) targetPos.y = maxY;

        if (minX > maxX) targetPos.x = mapBounds.position.x + mapBounds.size.x / 2.f;
        if (minY > maxY) targetPos.y = mapBounds.position.y + mapBounds.size.y / 2.f;

        mWorldView.setCenter(targetPos);
    }
}

void Game::render() {
    mWindow.clear(sf::Color(135, 206, 235));
    mWindow.setView(mWorldView);

    if (mState == GameState::ROAMING) {
        mWorld.draw(mWindow);
    }
    else if (mState == GameState::INSIDE_HOUSE) {
        mWindow.clear(sf::Color::Black);
        mWorld.getInterior().draw(mWindow);
        
        mWorld.drawPlayer(mWindow);

        // --- DEBUG DOT ---
        CircleShape dot(3);
        dot.setFillColor(sf::Color::Red);
        dot.setOrigin({3, 3});
        dot.setPosition(mWorld.getPlayerPosition());
        mWindow.draw(dot);
        // -----------------
    }

    mWindow.setView(mWindow.getDefaultView());

    if (mShowComputerUI) {
        mWindow.draw(mMonitorFrame);
        mWindow.draw(mMonitorScreen);
        mWindow.draw(mComputerText);

        Text computerTimer = mTimerText;
        computerTimer.setPosition({340.f, 250.f});
        computerTimer.setCharacterSize(60);
        computerTimer.setFillColor(sf::Color::Green);
        mWindow.draw(computerTimer);

        Text computerStatus = mStatusText;
        computerStatus.setPosition({320.f, 320.f});
        computerStatus.setFillColor(sf::Color::Green); 
        if (mIsFocussing) computerStatus.setString("SESSION ACTIVE...");
        else computerStatus.setString("PAUSED");
        mWindow.draw(computerStatus);
    }

    if (mShowDoorPrompt || mShowExitPrompt) {
        if (mShowDoorPrompt) {
            mPromptText.setString("Enter House?\n\n[Y] Yes    [N] No");
        } 
        else if (mShowExitPrompt) {
            mPromptText.setString("Leave House?\n\n[Y] Yes    [N] No");
        }

        sf::FloatRect textBounds = mPromptText.getLocalBounds();
        mPromptText.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
        mPromptText.setPosition({400.f, 500.f});

        mWindow.draw(mPromptBox);
        mWindow.draw(mPromptText);
    }

    if  (mIsPaused) {
        mWindow.draw(mMenuBackrgound);
        mWindow.draw(mResumeText);
        mWindow.draw(mQuitText);
    }

    mWindow.display();
}