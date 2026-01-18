#include <SFML/Graphics.hpp>
#include <optional>
#include "Game.h"
using namespace std;
using namespace sf;

Game::Game() 
        : mWindow(VideoMode({800,600}), "Focus Garden")
{
    mWindow.setFramerateLimit(60);
    mWorld.init();
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
            mWindow.close();
        }
        else if (const auto* mousePress = event->getIf<Event::MouseButtonPressed>()) {
            if (mousePress->button == Mouse::Button::Left) {
                Vector2i mousePos = mousePress->position;
                Vector2i gridPos = mWorld.isoToGrid(static_cast<float>(mousePos.x),
                                                    static_cast<float>(mousePos.y));
                mWorld.toggleTree(gridPos.x, gridPos.y);
            }
        }
    }
}

void Game::update(Time dt) {
    mWorld.update(dt);

}

void Game::render() {
    mWindow.clear(sf::Color(135, 206, 235));
    mWorld.draw(mWindow);
    mWindow.display();
}