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
        mComputerText(mFont),
        mEditModeText(mFont),
        mWarningText(mFont),
        mWarningYesText(mFont),
        mWarningNoText(mFont),
        mSceneSprite(mSceneTexture.getTexture())

{
    mWindow.setFramerateLimit(60);
    mIsPaused = false;
    mMenuBackrgound.setSize(Vector2f({800,600}));
    mMenuBackrgound.setFillColor(Color(0, 0, 0, 150));

    if (!mSceneTexture.resize({800, 600})) {
        cerr << "Failed to create scene texture" << endl;
    }
    mSceneTexture.setSmooth(false);

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

    mComputerText.setString("SYSTEM READY\nHit [SPACE] to Start Focus");

    mEditModeText.setString("LEVEL EDITOR");
    mEditModeText.setCharacterSize(40);
    mEditModeText.setFillColor(Color::White);
    sf::FloatRect eBounds = mEditModeText.getLocalBounds();
    mEditModeText.setOrigin({eBounds.size.x / 2.f, eBounds.size.y / 2.f});
    mEditModeText.setPosition({400.f, 270.f});

    mWarningBox.setSize({500.f, 200.f});
    mWarningBox.setFillColor(sf::Color(40, 40, 40, 250));
    mWarningBox.setOutlineColor(sf::Color(200, 50, 50));
    mWarningBox.setOutlineThickness(3.f);
    mWarningBox.setOrigin({250.f, 100.f});
    mWarningBox.setPosition({400.f, 300.f});

    mWarningText.setString("WARNING: Focus Session Active!\n\nQuitting now will DESTROY your tree\nand all progress will be LOST.");
    mWarningText.setCharacterSize(18);
    mWarningText.setFillColor(sf::Color(255, 200, 200));
    sf::FloatRect wBounds = mWarningText.getLocalBounds();
    mWarningText.setOrigin({wBounds.size.x / 2.f, wBounds.size.y / 2.f});
    mWarningText.setPosition({200.f, 220.f  });

    mWarningYesText.setString("[Y] QUIT & DESTROY");
    mWarningYesText.setCharacterSize(20);
    mWarningYesText.setFillColor(sf::Color(255, 100, 100));
    sf::FloatRect yBounds = mWarningYesText.getLocalBounds();
    mWarningYesText.setOrigin({yBounds.size.x / 2.f, yBounds.size.y / 2.f});
    mWarningYesText.setPosition({400.f, 330.f});

    mWarningNoText.setString("[N] STAY FOCUSED");
    mWarningNoText.setCharacterSize(20);
    mWarningNoText.setFillColor(sf::Color(100, 255, 100));
    sf::FloatRect nBounds = mWarningNoText.getLocalBounds();
    mWarningNoText.setOrigin({nBounds.size.x / 2.f, nBounds.size.y / 2.f});
    mWarningNoText.setPosition({400.f, 360.f});

    mShowQuitWarning = false;

    setupCallbacks();

    try {
        mWorld.init();
        mWorld.initInterior();

        mWorld.load("garden.dat");

        mWorld.getPlayer().setCollissionCallback([this](Vector2f pos) {
            GameState state;
            if (state == GameState::INSIDE_HOUSE) {
            float width = 20.f;
            float depth = 12.f;

            float halfW = width / 2.f;
            float halfH = depth / 2.f;

            Vector2f corners[4] {
                { pos.x - halfW, pos.y - halfH },
                { pos.x + halfW, pos.y - halfH },
                { pos.x - halfW, pos.y + halfH },
                { pos.x + halfW, pos.y + halfH }
            };
            
            for (const auto& point : corners) {
                if (mWorld.isPositionBlocked(point, mState)) {
                    return true;
                }
            }
            return false;
        }
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

void Game::setupCallbacks() {
    mSessionManager.setOnStart([this](SessionType type) {
        if (type == SessionType::WORK) {
            cout << "Focus session started: tree will grow" << endl;
        }
    });

    mSessionManager.setOnComplete([this](SessionType type) {
        if (type == SessionType::WORK) {
            mWorld.completeTreeGrowth();
            mStatusText.setString("SESSION COMPLETE!");
        }
        else {
            mStatusText.setString("BREAK OVER - TIME TO FOCUS!");
        }
    });

    mSessionManager.setOnCancel([this](SessionType type) {
        if (type == SessionType::WORK) {
            mWorld.destroyActiveSapling();
            mStatusText.setString("SESSION CANCELLED - TREE DESTRYOYED!");
        }
    });
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

        bool inputDetected = false;

        if (event->is<Event::Closed>()) {
            mWorld.save("garden.dat");
            mWindow.close();
        }

        if (event->is<Event::KeyPressed>() ||
            event->is<Event::MouseButtonPressed>()) {
            inputDetected = true;
        }

        if (inputDetected) {
            if (mTimeSinceInput > IDLE_THRESHOLD) {
                mTimeSinceInput = 0.f;
                continue;
            }
            mTimeSinceInput = 0.f;
        }

        switch (mState) {
            case GameState::QUIT_WARNING:
                handleWarningInput(*event);
                break;
            case GameState::COMPUTER_IDLE:
            case GameState::COMPUTER_FOCUSSING:
                handleComputerInput(*event);
                break;
            case GameState::PAUSE_MENU:
                handlePauseMenuInput(*event);
                break;
            default:
                handleRoamingInput(*event);
                break;
        }
    }
}

void Game::handleComputerInput(const Event& event) {
    if (const auto* keyPress = event.getIf<Event::KeyPressed>()) {
        if (keyPress->scancode == Keyboard::Scancode::Escape) {
            if (mSessionManager.isActive()) {
                mShowQuitWarning = true;
                mState = GameState::QUIT_WARNING;
            }
            else {
            mShowComputerUI = false;
            mState = GameState::INSIDE_HOUSE;

            Vector2u winSize = mWindow.getSize();
            float w = static_cast<float>(winSize.x);
            float h = static_cast<float>(winSize.y);
            FloatRect roomBounds = mWorld.getInterior().getBounds();
            float desiredZoom = roomBounds.size.y / (w / 2.f);
            if (desiredZoom < 0.1f) desiredZoom = 1.f;
            mWorldView.setSize({w * desiredZoom, h * desiredZoom});
            }
        }
        else if (keyPress->scancode == Keyboard::Scancode::Space) {
            if (mSessionManager.isIdle()) {
                SessionType nextType = mSessionManager.getNextSessionType();
                mSessionManager.startSession(nextType, mConfigurableTimer);
                mState = GameState::COMPUTER_FOCUSSING;
            }
            else if (mSessionManager.isActive()) {
                mSessionManager.pauseSession();
            }
            else if (mSessionManager.isPaused()) {
                mSessionManager.resumeSession();
            }
        }
        else if ((keyPress->scancode == Keyboard::Scancode::Up || 
                keyPress-> scancode == Keyboard::Scancode::Right) &&
                mSessionManager.isIdle()) {
                    mConfigurableTimer += 60.f;
                    if (mConfigurableTimer > 5940.f) mConfigurableTimer = 5940.f;
        }
        else if ((keyPress->scancode == Keyboard::Scancode::Down ||
                keyPress->scancode == Keyboard::Scancode::Left) &&
                mSessionManager.isIdle()) {
                    mConfigurableTimer -= 60.f;
                    if (mConfigurableTimer < 60.f) mConfigurableTimer = 60.f;
                }
            }
        }

void Game::handleWarningInput(const Event& event) {
    if (const auto* keyPress = event.getIf<Event::KeyPressed>()) {
        if (keyPress->scancode == Keyboard::Scancode::Y) {
            mSessionManager.cancelSession();
            mShowQuitWarning = false;
            mShowComputerUI = false;
            mState = GameState::INSIDE_HOUSE;
            
            Vector2u winSize = mWindow.getSize();
            float w = static_cast<float>(winSize.x);
            float h = static_cast<float>(winSize.y);
            FloatRect roomBounds = mWorld.getInterior().getBounds();
            float desiredZoom = roomBounds.size.y / (w / 2.f);
            if (desiredZoom < 0.1f) desiredZoom = 1.f;
            mWorldView.setSize({w * desiredZoom, h * desiredZoom});
        }
        else if (keyPress->scancode == Keyboard::Scancode::N ||
                 keyPress->scancode == Keyboard::Scancode::Escape) {
                mShowQuitWarning = false;
                mState = mSessionManager.isActive() ? 
                    GameState::COMPUTER_FOCUSSING : 
                    GameState::COMPUTER_IDLE;
        }
    }
}

void Game::handleRoamingInput(const Event& event) {
    if (mShowDoorPrompt) {
        if (const auto* keyPress = event.getIf<Event::KeyPressed>()) {
            if (keyPress->scancode == Keyboard::Scancode::Y ||
                keyPress->scancode == Keyboard::Scancode::Enter) {
                    
                    mShowDoorPrompt = false;
                    mState = GameState::INSIDE_HOUSE;
                    mWorld.setPlayerPosition(mWorld.getInterior().IgridToIso(13, 13));
                    mWorld.disablePlayerCollision();
                    Vector2u winSize = mWindow.getSize();
                    float w = static_cast<float>(winSize.x);
                    float h = static_cast<float>(winSize.y);
                    FloatRect roomBounds = mWorld.getInterior().getBounds();
                    float desiredZoom = roomBounds.size.y / (w / 2.f);
                    if (desiredZoom < 0.1f) desiredZoom = 1.f; 
                    mWorldView.setSize({w * desiredZoom , h * desiredZoom});
                    Vector2f center = roomBounds.getCenter();
                    center.y -= 70.f;
                    mWorldView.setCenter(center);
                    cout << "Entered house" << endl;
                }
            else if (keyPress->scancode == Keyboard::Scancode::N ||
                        keyPress->scancode == Keyboard::Scancode::Escape) {
    
                        mShowDoorPrompt = false;
                        Vector2f currentPos = mWorld.getPlayerPosition();
                        mWorld.setPlayerPosition({currentPos.x, currentPos.y + 15.f});
            }
        }
        return;
    }
    if (mShowExitPrompt) {
        if (const auto* keyPress = event.getIf<Event::KeyPressed>()) {
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
        return;
    }

    if (const auto* resized = event.getIf<Event::Resized>()) {
        handleWindowResize(*resized);
        return;
    }

    if (const auto* mousePress = event.getIf<Event::MouseButtonPressed>()) {
        Vector2i pixelPos = mousePress->position;
        Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos, mWorldView);
        if (mousePress->button == Mouse::Button::Left) {
            if (!mSessionManager.isActive()) {
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
                    mIsEditing = false;
                }
                else if (mEditModeText.getGlobalBounds().contains(worldPos)) {
                    mIsPaused = false;
                    mIsEditing = true;
                    cout << "Editing mode ON" << endl;
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
            if (mIsEditing) {
                bool handled = mWorld.getInterior().handleEditorInput(mWindow, mWorldView, event);
                if (handled) return;
            }
        }
    }
    else if (const auto* mouseMove = event.getIf<Event::MouseMoved>()) {
        if (!mSessionManager.isActive()) {
            Vector2i pixelPos = mouseMove->position;
            Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos, mWorldView);
            Vector2i gridPos = mWorld.isoToGrid(worldPos.x, worldPos.y);
    
            mWorld.setHoveredTile(gridPos);
        }
        else {
            mWorld.setHoveredTile({-1, -1});
        }
        
    }
    else if (const auto* keyPress = event.getIf<Event::KeyPressed>()) {
        if (keyPress->scancode == Keyboard::Scancode::Enter) {
            if (!mSessionManager.isActive()) {
                if (mState == GameState::INSIDE_HOUSE &&
                    mWorld.getInterior().isComputer(mWorld.getPlayerPosition())) {
                        mShowComputerUI = true;
                        mState = GameState::COMPUTER_IDLE;
                    }
                else {
                    mWorld.interact();
                }
            }
        }
        else if (keyPress->scancode == Keyboard::Scancode::Escape) {
            if (mIsEditing) {
                mIsEditing = false;
                cout << "Editing mode OFF" << endl;
            }
            else {
                mIsPaused = !mIsPaused;
            }
        }
    }  
}
void Game::handleWindowResize(const Event::Resized& resized) {
    float w = static_cast<float>(resized.size.x);
    float h = static_cast<float>(resized.size.y);

   if (!mSceneTexture.resize({resized.size.x, resized.size.y})) {
        cerr << "Failed to reize scene texture" << endl;
   }
    
    mUIView.setSize({w, h});
    mUIView.setCenter({w / 2.f, h / 2.f});
    FloatRect currentMapBounds;
    if (mState == GameState::INSIDE_HOUSE ||
        mState == GameState::COMPUTER_IDLE ||
        mState == GameState::COMPUTER_FOCUSSING ||
        mState == GameState::QUIT_WARNING) {
        currentMapBounds = mWorld.getInterior().getBounds();
    }
    else {
        currentMapBounds = mWorld.getBounds();
    }
    float mapHeight = currentMapBounds.size.y;
    float desiredZoom = mapHeight / (w / 2.f);
    mWorldView.setSize({w * desiredZoom, h * desiredZoom});
    Vector2f finalCenter = currentMapBounds.getCenter();
    if (mState == GameState::INSIDE_HOUSE ||
        mState == GameState::COMPUTER_IDLE ||
        mState == GameState::COMPUTER_FOCUSSING ||
        mState == GameState::QUIT_WARNING) {
        finalCenter.y -= 70.f;
    }
    mWorldView.setCenter(finalCenter);
}

void Game::update(Time dt) {
    if (mIsPaused) {
        return;
    }
    if (mShowDoorPrompt || mShowExitPrompt) {
        return;
    }
    
    mSessionManager.update(dt);
    mWorld.update(dt,
                  mSessionManager.isActive(),
                  mSessionManager.getProgress());
                  
    if (mShowComputerUI) {
        updateComputerUIText();
        mTimeSinceInput += dt.asSeconds();
    }
    
    Vector2f targetPos = mWorld.getPlayerPosition();
                    
    if (mState == GameState::ROAMING) {
        if (mWorld.checkDoorEntry(mWorld.getPlayerPosition())) {
            mShowDoorPrompt = true;
            return;
        }

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
    else if (mState == GameState::INSIDE_HOUSE ||
             mState == GameState::COMPUTER_IDLE ||
             mState == GameState::COMPUTER_FOCUSSING ||
             mState == GameState::QUIT_WARNING) {

        if (mIsEditing) {
            mWorld.getInterior().updateEditor(mWindow, mWorldView);
        }
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
}

void Game::updateComputerUIText() {
    float timeToDisplay = mSessionManager.isIdle() ?
    mConfigurableTimer :
    mSessionManager.getTimeRemaining();

    int minutes = static_cast<int>(timeToDisplay) / 60;
    int seconds = static_cast<int>(timeToDisplay) % 60;
    
    string minStr = (minutes < 10 ? "0" : "") + to_string(minutes);
    string secStr = (seconds < 10 ? "0" : "") + to_string(seconds);

    int sessionCount = mSessionManager.getCompletedFocusSessions();
    
    string stateLabel;
    SessionType type = mSessionManager.getType();
    if (type == SessionType::WORK) stateLabel = "FOCUS SESSION";
    else if (type == SessionType::SHORT_BREAK) stateLabel = "SHORT BREAK";
    else if (type == SessionType::LONG_BREAK) stateLabel = "LONG BREAK";
    else stateLabel = "READY";
    
    string status;
    if (mSessionManager.isActive()) {
        status = "RUNNING... (SPACE TO PAUSE)";
    }
    else if (mSessionManager.isPaused()) {
        status = "PAUSED (SPACE TO RESUME)";
    }
    else {
        status = "[ARROWS] ADJUST TIME\n[SPACE] START";
    }

    mComputerText.setString(stateLabel + "\n" + 
                           minStr + ":" + secStr + "\n" +
                           "Sessions: " + to_string(sessionCount) + "\n\n" + 
                           status);
}

void Game::render() {
    
    mSceneTexture.clear(Color(135, 206, 235));
    mSceneTexture.setView(mWorldView);

    if (mState == GameState::ROAMING) {
        mWorld.draw(mSceneTexture);
    }
    else if (mState == GameState::INSIDE_HOUSE) {
        mWindow.clear(sf::Color::Black);

        if (!mShowComputerUI) {
            Vector2f playerPos = mWorld.getPlayerPosition();
            mWorld.getInterior().draw(
                mSceneTexture,
                playerPos,
                [&]() { 
                    mWorld.drawPlayer(mSceneTexture);
                    float w = 20.f;
                    float h = 12.f;
                    Vector2f pos = mWorld.getPlayerPosition();

                    RectangleShape box({w, h});
                    box.setOrigin({w / 2.f, h / 2.f});
                    box.setPosition(pos);
                    box.setFillColor(Color::Transparent);
                    box.setOutlineColor(Color::Yellow);
                    box.setOutlineThickness(1.f);
                    mWindow.draw(box);
                },
                mIsEditing
                );
            //mWorld.drawPlayer(mWindow);

            // --- DEBUG DOT ---
            CircleShape dot(3);
            dot.setFillColor(sf::Color::Red);
            dot.setOrigin({3, 3});
            dot.setPosition(mWorld.getPlayerPosition());
            mSceneTexture.draw(dot);
            // -----------------
        }
    }
    mSceneTexture.display();
    
    mWindow.clear();
    mWindow.setView(mWindow.getDefaultView());
    
    mSceneSprite.setTexture(mSceneTexture.getTexture(), true);
    if (mState == GameState::INSIDE_HOUSE) {
        mSceneSprite.setColor(Color::White);
    }
    else {
        mSceneSprite.setColor(mWorld.getAmbientLight());
    }
    mWindow.draw(mSceneSprite);

    if (mState == GameState::ROAMING) {
        mWindow.setView(mWorldView);

        RenderStates glowState;
        glowState.blendMode = BlendAdd;
        mWorld.drawParticle(mWindow);
        mWindow.setView(mWindow.getDefaultView());
    }

    if (mShowComputerUI) {
        float alpha = 255.f;
        float backgroundAlpha = 0.f;
        
        if (mTimeSinceInput > IDLE_THRESHOLD) {
            View savedView = mWorldView;
            Vector2u winSize = mWindow.getSize();

            float w = static_cast<float>(winSize.x);
            float h = static_cast<float>(winSize.y);

            FloatRect gardenBounds = mWorld.getBounds();
            float desiredZoom = gardenBounds.size.y / (w / 2.f);

            if (desiredZoom <= 0.f) desiredZoom = 1.0f;
            
            mWorldView.setSize({w * desiredZoom, h * desiredZoom});
            mWorldView.setCenter(gardenBounds.getCenter());
            
            mWindow.setView(mWorldView);
            mWindow.clear(sf::Color(135, 206, 235));
            mWorld.draw(mWindow);
            mWorldView = savedView;
            mWindow.setView(mWindow.getDefaultView());

            float fadeTime = 2.f;
            float progress = (mTimeSinceInput - IDLE_THRESHOLD) / fadeTime;
            if (progress > 1.f) progress = 1.f;

            alpha = 255.f * (1.f - progress);
            backgroundAlpha = 255.f * progress;

            RectangleShape gardenOverlay({4000.f, 2000.f});
            gardenOverlay.setFillColor(Color(0, 0, 0, static_cast<uint8_t>(255 - backgroundAlpha)));
            mWindow.draw(gardenOverlay);
        }
        else {
            RectangleShape fullScreenBg({4000.f, 2000.f});
            fullScreenBg.setFillColor(Color::Black);
            mWindow.draw(fullScreenBg);
        }

        if (alpha > 1.f) {
            sf::Color frameCol = mMonitorFrame.getFillColor();
            sf::Color screenCol = mMonitorScreen.getFillColor();
            sf::Color textCol = mComputerText.getFillColor();
            sf::Color outlineCol = mMonitorFrame.getOutlineColor();

            mMonitorFrame.setFillColor({frameCol.r, frameCol.g, frameCol.b, static_cast<uint8_t>(alpha)});
            mMonitorFrame.setOutlineColor({outlineCol.r, outlineCol.g, outlineCol.b, static_cast<uint8_t>(alpha)});
            mMonitorScreen.setFillColor({screenCol.r, screenCol.g, screenCol.b, static_cast<uint8_t>(alpha)});
            mComputerText.setFillColor({textCol.r, textCol.g, textCol.b, static_cast<uint8_t>(alpha)});

            mWindow.draw(mMonitorFrame);
            mWindow.draw(mMonitorScreen);
            mWindow.draw(mComputerText);

            mMonitorFrame.setFillColor({frameCol.r, frameCol.g, frameCol.b, 255});
            mMonitorFrame.setOutlineColor({outlineCol.r, outlineCol.g, outlineCol.b, 255});
            mMonitorScreen.setFillColor({screenCol.r, screenCol.g, screenCol.b, 255});
            mComputerText.setFillColor({textCol.r, textCol.g, textCol.b, 255});

        }

        Text computerTimer = mTimerText;
        computerTimer.setPosition({340.f, 250.f});
        computerTimer.setCharacterSize(60);
        computerTimer.setFillColor(sf::Color::Green);
        mWindow.draw(computerTimer);

        Text computerStatus = mStatusText;
        computerStatus.setPosition({320.f, 320.f});
        computerStatus.setFillColor(sf::Color::Green); 
        if (mSessionManager.isActive() && mTimeSinceInput <= IDLE_THRESHOLD) {
             computerStatus.setString("SESSION ACTIVE...");
            mWindow.draw(computerStatus);
        }
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

    if (mShowQuitWarning && mState == GameState::QUIT_WARNING) {
    mWindow.draw(mWarningBox);
    mWindow.draw(mWarningText);
    mWindow.draw(mWarningYesText);
    mWindow.draw(mWarningNoText);
}

    if  (mIsPaused) {
        mWindow.draw(mMenuBackrgound);
        mWindow.draw(mResumeText);
        mWindow.draw(mEditModeText);
        mWindow.draw(mQuitText);
    }

    mWindow.display();
}

void Game::handlePauseMenuInput(const Event& event) {
    if (const auto* keyPress = event.getIf<Event::KeyPressed>()) {
        if (keyPress->scancode == Keyboard::Scancode::Escape) {
            mIsPaused = false;
            mState = GameState::ROAMING;
        }
    }
    else if (const auto* mousePress = event.getIf<Event::MouseButtonPressed>()) {
        if (mousePress->button == Mouse::Button::Left) {
            Vector2i mousePos = Mouse::getPosition(mWindow);
            Vector2f worldPos = mWindow.mapPixelToCoords(mousePos, mWindow.getDefaultView());
            
            if (mResumeText.getGlobalBounds().contains(worldPos)) {
                mIsPaused = false;
                mState = GameState::ROAMING;
            }
            else if (mEditModeText.getGlobalBounds().contains(worldPos)) {
                mIsPaused = false;
                mIsEditing = true;
                mState = GameState::INSIDE_HOUSE;
                cout << "Editing mode ON" << endl;
            }
            else if (mQuitText.getGlobalBounds().contains(worldPos)) {
                mWorld.save("garden.dat");
                mWindow.close();
            }
        }
    }
}

