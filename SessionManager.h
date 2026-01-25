#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "GameState.h"

class SessionManager {
    public:
        SessionManager();

        void startSession(SessionType, float duration);
        void pauseSession();
        void resumeSession();
        void completeSession();
        void cancelSession();

        void update(sf::Time dt);

        SessionState getState() const { return mState; }
        SessionType getType() const { return mType; }
        float getTimeRemaining() const { return mTimeRemaining; }
        float getDuration() const { return mDuration; }
        float getProgress() const {
            return mDuration > 0.f ? (mDuration - mTimeRemaining) / mDuration : 0.f;
        }
        int getCompletedFocusSessions() const { return mCompletedFocusSessions; }

        bool isActive() const { return mState == SessionState::RUNNING; }
        bool isPaused() const { return mState == SessionState::PAUSED; }
        bool isIdle() const { return mState == SessionState::IDLE; }

        SessionType getNextSessionType() const;
        float getDefaultDuration(SessionType type) const;

        void setOnComplete(std::function<void(SessionType)> callback) {
            mOnComplete = callback;
        }
        void setOnCancel(std::function<void(SessionType)> callback) {
            mOnCancel = callback;
        }
        void setOnStart(std::function<void(SessionType)> callback) {
            mOnStart = callback;
        }

        void setConfig(const SessionConfig& config) { mConfig = config; }
        const SessionConfig& getConfig() const { return mConfig; }

    private:
        SessionState mState;
        SessionType mType;
        float mTimeRemaining;
        float mDuration;
        int mCompletedFocusSessions;

        SessionConfig mConfig;

        std::function<void(SessionType)> mOnComplete;
        std::function<void(SessionType)> mOnCancel;
        std::function<void(SessionType)> mOnStart;
};