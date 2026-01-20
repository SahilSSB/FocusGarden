#pragma once 
#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include "GameState.h"
using namespace std;

class Player {
    public:
        Player();
        void init(const string& texturePath);
        void update(sf::Time dt, GameState currentState);
        void draw(sf::RenderTarget& target);
        int getDirection() const { return mDirectionIndex; }

        sf::Vector2f getPosition() const { return mSprite.getPosition(); }
        void setPosition(sf::Vector2f pos) { mSprite.setPosition(pos); }
        sf::FloatRect getBounds() const { return mSprite.getGlobalBounds(); }
        void setCollissionCallback(function<bool(sf::Vector2f)> callback) {
            mCollisionCheck = callback;
        }

    
    private:
        sf::Sprite mSprite;
        sf::Texture mTexture;

        //sprite settings
        const int FRAME_WIDTH = 16;
        const int FRAME_HEIGHT = 22;
        const int NUM_FRAMES = 3;

        int characterStartY = 31;

        //state
        sf::Vector2f mVelocity;
        float mSpeed = 150.f;
   

        //animation
        int mDirectionIndex = 0;
        float mAnimationTimer = 0.f;
        int mCurrentFrame = 0.f;
        float mFrameDuration = 0.1f;

        function<bool(sf::Vector2f)> mCollisionCheck;

        //helpers
        void updateAnimation(sf::Time dt);
        void handleInput(GameState state);

};