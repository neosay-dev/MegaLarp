#pragma once

#include <array>
#include <string>

namespace megalarp {

enum class TrailMode : int {
    Normal = 0,
    AlwaysOn = 1,
    AlwaysOff = 2
};

enum class ThemePreset : int {
    MegaLarpDark = 0,
    CyberPurple = 1,
    ClassicGreen = 2,
    CrimsonRed = 3
};

struct RuntimeState {
    bool cheatedThisAttempt = false;
    bool inLevel = false;
    bool playerDead = false;

    int noclipDeaths = 0;
    int noclipTotalFrames = 0;
    int noclipSafeFrames = 0;
    float noclipTintAlpha = 0.0f;

    int startPosCount = 0;
    int currentStartPosIndex = -1; // -1 = beginning of level
    int pendingStartPosDelta = 0;  // -1 prev, +1 next

    float currentFps = 0.0f;
    float rainbowHue = 0.0f;

    void resetAttempt() {
        cheatedThisAttempt = false;
        playerDead = false;
        noclipDeaths = 0;
        noclipTotalFrames = 0;
        noclipSafeFrames = 0;
        noclipTintAlpha = 0.0f;
    }

    float getNoclipAccuracy() const {
        if (noclipTotalFrames <= 0) return 100.0f;
        return 100.0f * static_cast<float>(noclipSafeFrames) / static_cast<float>(noclipTotalFrames);
    }
};

struct Config {
    // --- Gameplay ---
    bool noclip = false;
    bool noclipTintOnDeath = true;
    bool noclipAccuracy = true;
    bool safeMode = false;
    bool autoSafeMode = true;
    bool speedhackEnabled = false;
    float speedhackValue = 1.0f;
    bool speedhackAudio = true;
    bool practiceMusicSync = false;
    bool autoPracticeMode = false;
    bool instantComplete = false;
    bool jumpHack = false;
    bool instantRestart = false;
    bool noRespawnBlink = false;
    bool noDeathEffect = false;
    bool startPosSwitcher = true;
    bool noMirrorEffect = false;

    // --- Visual & FPS ---
    bool fpsBypassEnabled = false;
    float fpsBypassValue = 240.0f;
    bool showHitboxes = false;
    bool showHitboxesOnDeath = false;
    bool hidePauseMenu = false;
    bool hidePracticeButtons = false;
    bool hideAttempts = false;
    bool noWavePulse = false;
    bool noGlow = false;
    int trailMode = static_cast<int>(TrailMode::Normal);
    bool noCameraShake = false;
    bool rainbowIcon = false;
    float rainbowSpeed = 1.0f;

    // --- Creator & Levels ---
    bool copyHack = false;
    bool verifyHack = false;
    bool objectLimitBypass = false;
    bool customObjectBypass = false;
    bool sliderLimitBypass = false;
    bool levelEdit = false;
    bool defaultSongBypass = false;

    // --- Global & QoL ---
    bool unlockAllIcons = false;
    bool noTransition = false;
    bool fastMenu = false;
    bool confirmExit = false;
    bool cheatIndicator = true;
    bool showFpsCounter = false;
    bool showMobileButton =
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
        true;
#else
        false;
#endif

    // --- UI & Theme ---
    int themePreset = static_cast<int>(ThemePreset::MegaLarpDark);
    float menuScale = 1.0f;
    float menuOpacity = 0.94f;
    bool tabbedMode = false;

    RuntimeState runtime;

    static Config& get();

    void load();
    void save() const;
    void resetDefaults();

    bool isCheating() const;
    bool isSafeModeEffective() const;
    void markCheatedIfNeeded();
};

} // namespace megalarp
