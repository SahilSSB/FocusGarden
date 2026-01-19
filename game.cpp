#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <iostream>
#include "Game.h"
using namespace std;
using namespace sf;

Game::Game() 
        : mWindow(VideoMode({800,600}), "Focus Garden"),
        mState(GameState::ROAMING),
        mTimerText(mFont),
        mStatusText(mFont)
{
    mWindow.setFramerateLimit(60);
    
    try {
        mWorld.init();

        mWorld.load("garden.dat");

        if (!mFont.openFromFile("fonts/JetBrainsMonoNLNerdFontMono-Regular.ttf")) {
            cerr << "WARNING: font not found" << endl;
        }
        mTimerText.setCharacterSize(40);
        mTimerText.setPosition({20.f, 20.f});
        mTimerText.setFillColor(Color::White);
        mTimerText.setOutlineColor(Color::Black);
        mTimerText.setOutlineThickness(2.f);
        mTimerText.setString("25:00");

        mStatusText.setCharacterSize(20);
        mStatusText.setPosition({20.f, 70.f});
        mStatusText.setFillColor(Color::Yellow);
        mStatusText.setOutlineColor(Color::Black);
        mStatusText.setOutlineThickness(1.f);
        mStatusText.setString("Press SPACE to start Focus");
        
        FloatRect mapBounds = mWorld.getBounds();
        mWorldView = mWindow.getDefaultView();
        mWorldView.setCenter(mapBounds.getCenter());
        mWorldView.zoom(1.2f);

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
        processEvents();
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
        else if (const auto* resized = event->getIf<Event::Resized>()) {
            float w = static_cast<float>(resized->size.x);
            float h = static_cast<float>(resized->size.y);
            
            mUIView.setSize({w, h});
            mUIView.setCenter({w / 2.f, h / 2.f});
            FloatRect mapBounds = mWorld.getBounds();
            float mapHeight = mapBounds.size.y;

            float desiredZoom = mapHeight / (w / 2.f);
            
            mWorldView.setSize({w * desiredZoom, h * desiredZoom});
            mWorldView.setCenter(mapBounds.getCenter());
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
            if (keyPress->scancode == Keyboard::Scancode::Space) {
                mIsFocussing = !mIsFocussing;
                if (mIsFocussing) {
                    mFocusTimer = TARGET_TIME;
                    mStatusText.setString("FOCUSSING... (Planting Locked)");
                    mStatusText.setFillColor(Color::Green);
                }
                else {
                    mStatusText.setString("Paused");
                    mStatusText.setFillColor(Color::Black);
                }
            }
        }
    }
}

void Game::update(Time dt) {
    if (mIsFocussing) {
        mFocusTimer -= dt.asSeconds();
        if (mFocusTimer <= 0.f) {
            mFocusTimer = 0.f;
            mIsFocussing = false;
            mStatusText.setString("SESSION COMPLETE!");
        }
    }
    int minutes = static_cast<int>(mFocusTimer) / 60;
    int seconds = static_cast<int>(mFocusTimer) % 60;
    string timeStr = to_string(minutes) + ":";
    if (seconds < 10) timeStr += "0";
    timeStr += to_string(seconds);
    mTimerText.setString(timeStr);
    
    mWorld.update(dt, mIsFocussing);
}

void Game::render() {
    mWindow.clear(sf::Color(135, 206, 235));
    mWindow.setView(mWorldView);
    mWorld.draw(mWindow);

    mWindow.setView(mUIView);
    mWindow.draw(mTimerText);
    mWindow.draw(mStatusText);

    mWindow.display();
}