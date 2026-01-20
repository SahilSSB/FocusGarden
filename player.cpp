#include "Player.h"
#include <iostream>
using namespace std;
using namespace sf;

const float PI = 3.14159265f;

Player::Player() : mSprite(mTexture) {

}

void Player::init(const string& texturePath) {
    if (!mTexture.loadFromFile(texturePath)) {
        throw runtime_error("Could not load Texture. Is it in the correct folder?");
    }
    mSprite.setTexture(mTexture);
    mSprite.setTextureRect(IntRect({0, 0}, {FRAME_WIDTH, FRAME_HEIGHT}));
    mSprite.setOrigin({FRAME_WIDTH / 2.f, static_cast<float>(FRAME_HEIGHT)});
    mSprite.setScale({1.8f, 1.8f});
}

void Player::update(Time dt, GameState currentState) {
    if (currentState != GameState::ROAMING || (mVelocity.x == 0 && mVelocity.y == 0)) {
        if (currentState == GameState::ROAMING) handleInput();
        if (mVelocity.x == 0 && mVelocity.y == 0) {
            mCurrentFrame = 0;
            int sheetColumn = 0;
            switch(mDirectionIndex) {
                case 0: sheetColumn = 2; break;
                case 1: sheetColumn = 3; break;
                case 2: sheetColumn = 4; break;
                case 3: sheetColumn = 5; break;
                case 4: sheetColumn = 6; break;
                case 5: sheetColumn = 7; break;
                case 6: sheetColumn = 0; break;
                case 7: sheetColumn = 1; break;
        }
        int rectLeft = sheetColumn * FRAME_WIDTH;
        int rectTop = characterStartY;
        mSprite.setTextureRect(IntRect({rectLeft, rectTop}, {FRAME_WIDTH, FRAME_HEIGHT}));
        return;
    }
}
    handleInput();
    Vector2f newPos = mSprite.getPosition() + mVelocity * dt.asSeconds();
    
    if (mCollisionCheck && !mCollisionCheck(newPos)) {
        mSprite.setPosition(newPos);
    }

    if (mVelocity.x != 0 || mVelocity.y != 0) {
        updateAnimation(dt);
    }
    else {
        mCurrentFrame = 0;
        int sheetColumn = 0;
        switch(mDirectionIndex) {
            case 0: sheetColumn = 2; break; 
            case 1: sheetColumn = 3; break;
            case 2: sheetColumn = 4; break; 
            case 3: sheetColumn = 5; break;
            case 4: sheetColumn = 6; break; 
            case 5: sheetColumn = 7; break;
            case 6: sheetColumn = 0; break; 
            case 7: sheetColumn = 1; break;
        }
        int rectLeft = sheetColumn * FRAME_WIDTH;
        int rectTop = characterStartY;
        mSprite.setTextureRect(IntRect({rectLeft, rectTop}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }
}

void Player::handleInput() {
    mVelocity = {0.f, 0.f};

    float inputX = 0.f;
    float inputY = 0.f;
    
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) inputY -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) inputY += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) inputX -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) inputX += 1.f;

    if (inputX != 0 || inputY != 0) {
        mVelocity.x = (inputX - inputY);
        mVelocity.y = (inputX + inputY) * 0.5f;
    }
    if (mVelocity.x != 0 || mVelocity.y != 0) {
        float length = sqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y);
        mVelocity /= length;
        mVelocity *= mSpeed;

        float angle = atan2(mVelocity.y, mVelocity.x) * 180.f / PI;
        if (angle < 0) angle += 360.f;

        float step = 360.f / 8.f;
        mDirectionIndex = static_cast<int>((angle + (step / 2.f)) / step) % 8;
    }
}

void Player::updateAnimation(Time dt) {
    mAnimationTimer += dt.asSeconds();
    if (mAnimationTimer >= mFrameDuration) {
        mAnimationTimer = 0.f;

        mCurrentFrame = (mCurrentFrame + 1) % NUM_FRAMES;

        int sheetColumn = 0;

        switch(mDirectionIndex) {
            case 0: sheetColumn = 2; break;
            case 1: sheetColumn = 3; break;
            case 2: sheetColumn = 4; break;
            case 3: sheetColumn = 5; break;
            case 4: sheetColumn = 6; break;
            case 5: sheetColumn = 7; break;
            case 6: sheetColumn = 0; break;
            case 7: sheetColumn = 1; break;
        }

        int rectLeft = sheetColumn * FRAME_WIDTH;
        int rectTop = characterStartY + (mCurrentFrame * FRAME_HEIGHT);
        mSprite.setTextureRect(IntRect({rectLeft, rectTop}, {FRAME_WIDTH, FRAME_HEIGHT}));
    }
}

void Player::draw(RenderTarget& target) {
    target.draw(mSprite);
}