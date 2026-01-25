#pragma once

enum class GameState {
    ROAMING,
    FOCUSSING,
    MENU,
    INSIDE_HOUSE,
    COMPUTER_IDLE,
    COMPUTER_FOCUSSING,
    QUIT_WARNING,
    PAUSE_MENU
};

enum class SessionType {
    NONE,
    WORK,
    SHORT_BREAK,
    LONG_BREAK
};

enum class SessionState {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED,
    CANCELLED
};

struct SessionConfig {
    float focusDuration = 25.f * 60.f;
    float shortBreakDuration = 5.f * 60.f;
    float longBreakDuration = 15.f * 60.f;
    int sessionsUntilLongBreak = 4;
};