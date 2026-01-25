#include "SessionManager.h"
#include <iostream>
using namespace std;
using namespace sf;

SessionManager::SessionManager()
    : mState(SessionState::IDLE)
    , mType(SessionType::NONE)
    , mTimeRemaining(0.f)
    , mDuration(0.f)
    , mCompletedFocusSessions(0)
{}

void SessionManager::startSession(SessionType type, float duration) {
    if (mState == SessionState::RUNNING) {
        cerr << "Warning: Starting new session while one is active!" << endl;
    }
    mType = type;
    mDuration = duration;
    mTimeRemaining = duration;
    mState = SessionState::RUNNING;

    if (mOnStart) {
        mOnStart(type);
    }
    cout << "Session started: Type= " << static_cast<int>(type) << "Duration "
         << duration << "s" << endl;
}

void SessionManager::pauseSession() {
    if (mState == SessionState::RUNNING) {
        mState = SessionState::PAUSED;
        cout << "Session paused" << endl;
    }
}

void SessionManager::resumeSession() {
    if (mState == SessionState::PAUSED) {
        mState = SessionState::RUNNING;
        cout << "Session resumed" << endl;
    }
}

void SessionManager::completeSession() {
    if (mState != SessionState::RUNNING && mState != SessionState::PAUSED) {
        return;
    }
    SessionType completedType = mType;
    mState = SessionState::COMPLETED;

    if (mType == SessionType::WORK) {
        mCompletedFocusSessions++;
        cout << "Focus session completed! Total: " << mCompletedFocusSessions << endl;
    }

    if (mOnComplete) {
        mOnComplete(completedType);
    }

    mType = SessionType::NONE;
    mTimeRemaining = 0.f;
    mDuration = 0.f;
    mState = SessionState::IDLE;
}

void SessionManager::cancelSession() {
    if (mState == SessionState::IDLE) {
        return;
    }

    SessionType cancelledType = mType;
    mState = SessionState::CANCELLED;

    cout << "Session cancelled: Type= " << static_cast<int>(cancelledType) << endl;

    if (mOnCancel) {
        mOnCancel(cancelledType);
    }

    mType = SessionType::NONE;
    mTimeRemaining = 0.f;
    mDuration = 0.f;
    mState = SessionState::IDLE;
}

void SessionManager::update(Time dt) {
    if (mState != SessionState::RUNNING) {
        return;
    }
    mTimeRemaining -= dt.asSeconds();

    if (mTimeRemaining <= 0.f) {
        mTimeRemaining = 0.f;
        completeSession();
    }
}

SessionType SessionManager::getNextSessionType() const {
    if (mType == SessionType::WORK) {
        if (mCompletedFocusSessions % mConfig.sessionsUntilLongBreak == 0) {
            return SessionType::LONG_BREAK;
        }
        return SessionType::SHORT_BREAK;
    }
    return SessionType::WORK;
}

float SessionManager::getDefaultDuration(SessionType type) const {
    switch (type) {
        case SessionType::WORK:
            return mConfig.focusDuration;
        case SessionType::SHORT_BREAK:
            return mConfig.shortBreakDuration;
        case SessionType::LONG_BREAK:
            return mConfig.longBreakDuration;
        default:
            return mConfig.focusDuration;
    }
}