#include "Config.hpp"
#include <Geode/Geode.hpp>
#include <cmath>

using namespace geode::prelude;

namespace megalarp {

Config& Config::get() {
    static Config instance;
    return instance;
}

void Config::load() {
    auto* mod = Mod::get();

    // Gameplay
    noclip = mod->getSavedValue<bool>("gameplay.noclip", false);
    noclipTintOnDeath = mod->getSavedValue<bool>("gameplay.noclipTintOnDeath", true);
    noclipAccuracy = mod->getSavedValue<bool>("gameplay.noclipAccuracy", true);
    safeMode = mod->getSavedValue<bool>("gameplay.safeMode", false);
    autoSafeMode = mod->getSavedValue<bool>("gameplay.autoSafeMode", true);
    speedhackEnabled = mod->getSavedValue<bool>("gameplay.speedhackEnabled", false);
    speedhackValue = static_cast<float>(mod->getSavedValue<double>("gameplay.speedhackValue", 1.0));
    speedhackAudio = mod->getSavedValue<bool>("gameplay.speedhackAudio", true);
    practiceMusicSync = mod->getSavedValue<bool>("gameplay.practiceMusicSync", false);
    autoPracticeMode = mod->getSavedValue<bool>("gameplay.autoPracticeMode", false);
    instantComplete = mod->getSavedValue<bool>("gameplay.instantComplete", false);
    jumpHack = mod->getSavedValue<bool>("gameplay.jumpHack", false);
    instantRestart = mod->getSavedValue<bool>("gameplay.instantRestart", false);
    noRespawnBlink = mod->getSavedValue<bool>("gameplay.noRespawnBlink", false);
    noDeathEffect = mod->getSavedValue<bool>("gameplay.noDeathEffect", false);
    startPosSwitcher = mod->getSavedValue<bool>("gameplay.startPosSwitcher", true);
    noMirrorEffect = mod->getSavedValue<bool>("gameplay.noMirrorEffect", false);

    // Visual & FPS
    fpsBypassEnabled = mod->getSavedValue<bool>("visual.fpsBypassEnabled", false);
    fpsBypassValue = static_cast<float>(mod->getSavedValue<double>("visual.fpsBypassValue", 240.0));
    showHitboxes = mod->getSavedValue<bool>("visual.showHitboxes", false);
    showHitboxesOnDeath = mod->getSavedValue<bool>("visual.showHitboxesOnDeath", false);
    hidePauseMenu = mod->getSavedValue<bool>("visual.hidePauseMenu", false);
    hidePracticeButtons = mod->getSavedValue<bool>("visual.hidePracticeButtons", false);
    hideAttempts = mod->getSavedValue<bool>("visual.hideAttempts", false);
    noWavePulse = mod->getSavedValue<bool>("visual.noWavePulse", false);
    noGlow = mod->getSavedValue<bool>("visual.noGlow", false);
    trailMode = static_cast<int>(mod->getSavedValue<int64_t>("visual.trailMode", 0));
    noCameraShake = mod->getSavedValue<bool>("visual.noCameraShake", false);
    rainbowIcon = mod->getSavedValue<bool>("visual.rainbowIcon", false);
    rainbowSpeed = static_cast<float>(mod->getSavedValue<double>("visual.rainbowSpeed", 1.0));

    // Creator
    copyHack = mod->getSavedValue<bool>("creator.copyHack", false);
    verifyHack = mod->getSavedValue<bool>("creator.verifyHack", false);
    objectLimitBypass = mod->getSavedValue<bool>("creator.objectLimitBypass", false);
    customObjectBypass = mod->getSavedValue<bool>("creator.customObjectBypass", false);
    sliderLimitBypass = mod->getSavedValue<bool>("creator.sliderLimitBypass", false);
    levelEdit = mod->getSavedValue<bool>("creator.levelEdit", false);
    defaultSongBypass = mod->getSavedValue<bool>("creator.defaultSongBypass", false);

    // Global & QoL
    unlockAllIcons = mod->getSavedValue<bool>("global.unlockAllIcons", false);
    noTransition = mod->getSavedValue<bool>("global.noTransition", false);
    fastMenu = mod->getSavedValue<bool>("global.fastMenu", false);
    confirmExit = mod->getSavedValue<bool>("global.confirmExit", false);
    cheatIndicator = mod->getSavedValue<bool>("global.cheatIndicator", true);
    showFpsCounter = mod->getSavedValue<bool>("global.showFpsCounter", false);
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    showMobileButton = mod->getSavedValue<bool>("global.showMobileButton", true);
#else
    showMobileButton = mod->getSavedValue<bool>("global.showMobileButton", false);
#endif

    // UI
    themePreset = static_cast<int>(mod->getSavedValue<int64_t>("ui.themePreset", 0));
    menuScale = static_cast<float>(mod->getSavedValue<double>("ui.menuScale", 1.0));
    menuOpacity = static_cast<float>(mod->getSavedValue<double>("ui.menuOpacity", 0.94));
    tabbedMode = mod->getSavedValue<bool>("ui.tabbedMode", false);
}

void Config::save() const {
    auto* mod = Mod::get();

    // Gameplay
    mod->setSavedValue("gameplay.noclip", noclip);
    mod->setSavedValue("gameplay.noclipTintOnDeath", noclipTintOnDeath);
    mod->setSavedValue("gameplay.noclipAccuracy", noclipAccuracy);
    mod->setSavedValue("gameplay.safeMode", safeMode);
    mod->setSavedValue("gameplay.autoSafeMode", autoSafeMode);
    mod->setSavedValue("gameplay.speedhackEnabled", speedhackEnabled);
    mod->setSavedValue("gameplay.speedhackValue", static_cast<double>(speedhackValue));
    mod->setSavedValue("gameplay.speedhackAudio", speedhackAudio);
    mod->setSavedValue("gameplay.practiceMusicSync", practiceMusicSync);
    mod->setSavedValue("gameplay.autoPracticeMode", autoPracticeMode);
    mod->setSavedValue("gameplay.instantComplete", instantComplete);
    mod->setSavedValue("gameplay.jumpHack", jumpHack);
    mod->setSavedValue("gameplay.instantRestart", instantRestart);
    mod->setSavedValue("gameplay.noRespawnBlink", noRespawnBlink);
    mod->setSavedValue("gameplay.noDeathEffect", noDeathEffect);
    mod->setSavedValue("gameplay.startPosSwitcher", startPosSwitcher);
    mod->setSavedValue("gameplay.noMirrorEffect", noMirrorEffect);

    // Visual & FPS
    mod->setSavedValue("visual.fpsBypassEnabled", fpsBypassEnabled);
    mod->setSavedValue("visual.fpsBypassValue", static_cast<double>(fpsBypassValue));
    mod->setSavedValue("visual.showHitboxes", showHitboxes);
    mod->setSavedValue("visual.showHitboxesOnDeath", showHitboxesOnDeath);
    mod->setSavedValue("visual.hidePauseMenu", hidePauseMenu);
    mod->setSavedValue("visual.hidePracticeButtons", hidePracticeButtons);
    mod->setSavedValue("visual.hideAttempts", hideAttempts);
    mod->setSavedValue("visual.noWavePulse", noWavePulse);
    mod->setSavedValue("visual.noGlow", noGlow);
    mod->setSavedValue("visual.trailMode", static_cast<int64_t>(trailMode));
    mod->setSavedValue("visual.noCameraShake", noCameraShake);
    mod->setSavedValue("visual.rainbowIcon", rainbowIcon);
    mod->setSavedValue("visual.rainbowSpeed", static_cast<double>(rainbowSpeed));

    // Creator
    mod->setSavedValue("creator.copyHack", copyHack);
    mod->setSavedValue("creator.verifyHack", verifyHack);
    mod->setSavedValue("creator.objectLimitBypass", objectLimitBypass);
    mod->setSavedValue("creator.customObjectBypass", customObjectBypass);
    mod->setSavedValue("creator.sliderLimitBypass", sliderLimitBypass);
    mod->setSavedValue("creator.levelEdit", levelEdit);
    mod->setSavedValue("creator.defaultSongBypass", defaultSongBypass);

    // Global & QoL
    mod->setSavedValue("global.unlockAllIcons", unlockAllIcons);
    mod->setSavedValue("global.noTransition", noTransition);
    mod->setSavedValue("global.fastMenu", fastMenu);
    mod->setSavedValue("global.confirmExit", confirmExit);
    mod->setSavedValue("global.cheatIndicator", cheatIndicator);
    mod->setSavedValue("global.showFpsCounter", showFpsCounter);
    mod->setSavedValue("global.showMobileButton", showMobileButton);

    // UI
    mod->setSavedValue("ui.themePreset", static_cast<int64_t>(themePreset));
    mod->setSavedValue("ui.menuScale", static_cast<double>(menuScale));
    mod->setSavedValue("ui.menuOpacity", static_cast<double>(menuOpacity));
    mod->setSavedValue("ui.tabbedMode", tabbedMode);
}

void Config::resetDefaults() {
    auto savedRuntime = runtime;
    *this = Config{};
    runtime = savedRuntime;
    save();
}

bool Config::isCheating() const {
    bool speedActive = speedhackEnabled && std::fabs(speedhackValue - 1.0f) > 0.001f;
    return noclip
        || speedActive
        || instantComplete
        || jumpHack
        || showHitboxes
        || noMirrorEffect;
}

bool Config::isSafeModeEffective() const {
    if (safeMode) return true;
    if (autoSafeMode && (isCheating() || runtime.cheatedThisAttempt)) return true;
    return false;
}

void Config::markCheatedIfNeeded() {
    if (runtime.inLevel && isCheating()) {
        runtime.cheatedThisAttempt = true;
    }
}

} // namespace megalarp
