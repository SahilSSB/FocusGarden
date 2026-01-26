#include "Player.h"
#include <iostream>
using namespace std;
using namespace sf;

const float PI = 3.14159265f;

Player::Player() : mSprite(mTexture),
                   mStepSound(mDummyBuffer)
{}

void Player::init(const string& texturePath) {
    if (!mTexture.loadFromFile(texturePath)) {
        throw runtime_error("Could not load Texture. Is it in the correct folder?");
    }
    mSprite.setTexture(mTexture);

    SoundBuffer b1, b2;
    if (b1.loadFromFile("sounds/footsteps/walk_1.ogg")) mStepBuffer.push_back(b1);
    if (b2.loadFromFile("sounds/footsteps/walk_2.ogg")) mStepBuffer.push_back(b2);

    float frameWidth = 16.f;
    float frameHeight = 24.f;
    mSprite.setOrigin({frameWidth / 2.f, 16.f});
    mSprite.setTextureRect(IntRect({0, 0},{(int)frameWidth, (int)frameHeight}));
    mSprite.setScale({1.8f, 1.8f});
}

void Player::update(Time dt, GameState currentState) {
    if (currentState != GameState::ROAMING || (mVelocity.x == 0 && mVelocity.y == 0)) {
        if (currentState == GameState::ROAMING) handleInput(currentState);
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
    handleInput(currentState);
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

    bool isMoving = (mVelocity.x !=0 || mVelocity.y !=0);
    if (isMoving && (currentState == GameState::ROAMING && currentState != GameState::INSIDE_HOUSE)) {
        mStepTimer += dt.asSeconds();

        if (mStepTimer >= STEP_INTERVAL) {
            int idx = rand() % mStepBuffer.size();
            mStepSound.setBuffer(mStepBuffer[idx]);
            mStepSound.setPitch(1.f + (rand() % 20) / 100.f);
            mStepSound.setVolume(10.f);
            mStepSound.play();
            mStepTimer = 0.f;
        }
    }
    else {
        mStepTimer = STEP_INTERVAL;
    }

}

void Player::handleInput(GameState state) {
    mVelocity = {0.f, 0.f};

    float walkSpeed = mSpeed;

    if (Keyboard::isKeyPressed(Keyboard::Key::LShift)) walkSpeed = SPRINT_SPEED;
    else walkSpeed = mSpeed;

    float inputX = 0.f;
    float inputY = 0.f;
    
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) inputY -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) inputY += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) inputX -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) inputX += 1.f;

    if (state == GameState::INSIDE_HOUSE) {
        mVelocity = {inputX, inputY};
    }
    else{
        mVelocity.x = (inputX - inputY);
        mVelocity.y = (inputX + inputY) * 0.5f;
    }
    if (mVelocity.x != 0 || mVelocity.y != 0) {
        float length = sqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y);
        mVelocity /= length;
        mVelocity *= walkSpeed;

        float angle = atan2(mVelocity.y, mVelocity.x) * 180.f / PI;
        if (angle < 0) angle += 360.f;

        float step = 360.f / 8.f;
        mDirectionIndex = static_cast<int>((angle + (step / 2.f)) / step) % 8;
    }
}

void Player::updateAnimation(Time dt) {
    float multiplier = 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::LShift)) multiplier = 2.f;
    mAnimationTimer += dt.asSeconds() * multiplier;
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