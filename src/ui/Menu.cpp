#include "Menu.hpp"
#include "Theme.hpp"
#include "Keybinds.hpp"
#include "../config/Config.hpp"

#include <Geode/Geode.hpp>
#include <imgui.h>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

using namespace geode::prelude;

namespace megalarp {

namespace {

void applyFpsBypassNow() {
    auto& cfg = Config::get();
    auto* director = CCDirector::sharedDirector();
    if (!director) return;
    if (cfg.fpsBypassEnabled && cfg.fpsBypassValue >= 15.0f) {
        director->setAnimationInterval(1.0 / static_cast<double>(cfg.fpsBypassValue));
    } else {
        director->setAnimationInterval(1.0 / 60.0);
    }
}

bool icontains(const char* haystack, const char* needle) {
    if (!needle || !needle[0]) return true;
    if (!haystack) return false;
    std::string h(haystack);
    std::string n(needle);
    std::transform(h.begin(), h.end(), h.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(n.begin(), n.end(), n.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return h.find(n) != std::string::npos;
}

} // namespace

Menu& Menu::get() {
    static Menu instance;
    return instance;
}

void Menu::setup() {
    auto& cfg = Config::get();
    Theme::apply(static_cast<ThemePreset>(cfg.themePreset), cfg.menuOpacity, cfg.menuScale);
    m_themeDirty = false;
    Keybinds::get().init();
    applyFpsBypassNow();
}

bool Menu::matchesSearch(const char* text) const {
    return icontains(text, m_searchBuf);
}

bool Menu::checkbox(const char* label, bool* value, const char* tooltip, bool marksCheat) {
    if (!matchesSearch(label)) return false;
    bool changed = ImGui::Checkbox(label, value);
    if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::SetTooltip("%s", tooltip);
    }
    if (changed) {
        auto& cfg = Config::get();
        if (marksCheat) cfg.markCheatedIfNeeded();
        cfg.save();
    }
    return changed;
}

void Menu::render() {
    auto& cfg = Config::get();
    auto& kb = Keybinds::get();

    // Update FPS tracker
    cfg.runtime.currentFps = ImGui::GetIO().Framerate;

    if (m_themeDirty) {
        Theme::apply(static_cast<ThemePreset>(cfg.themePreset), cfg.menuOpacity, cfg.menuScale);
        m_themeDirty = false;
    }

    // Menu toggle hotkey
    if (kb.listeningId().empty() && kb.menuToggleKey() != ImGuiKey_None) {
        if (ImGui::IsKeyPressed(kb.menuToggleKey(), false)) {
            toggle();
        }
    }

    // Poll all other keybinds
    kb.poll(m_open);

    // Always-visible HUD overlays (noclip tint, cheat indicator, FPS, StartPos, mobile toggle)
    renderHudOverlays();

    if (!m_open) return;

    if (cfg.tabbedMode) {
        renderTabbedWindow();
    } else {
        renderFloatingWindows();
    }
}

void Menu::renderHudOverlays() {
    auto& cfg = Config::get();
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* bgDraw = ImGui::GetBackgroundDrawList();

    // 1. Noclip death tint overlay
    if (cfg.noclip && cfg.noclipTintOnDeath && cfg.runtime.inLevel && cfg.runtime.noclipTintAlpha > 0.005f) {
        ImU32 tintCol = ImGui::GetColorU32(ImVec4(0.95f, 0.12f, 0.12f, cfg.runtime.noclipTintAlpha * 0.38f));
        bgDraw->AddRectFilled(ImVec2(0.0f, 0.0f), io.DisplaySize, tintCol);
        cfg.runtime.noclipTintAlpha = std::max(0.0f, cfg.runtime.noclipTintAlpha - io.DeltaTime * 3.5f);
    }

    // 2. Cheat Indicator dot (top-left)
    if (cfg.cheatIndicator) {
        ImVec4 dotColor;
        if (cfg.isCheating()) {
            dotColor = cfg.isSafeModeEffective()
                ? ImVec4(0.98f, 0.78f, 0.18f, 0.95f)  // Yellow: cheating with Safe Mode
                : ImVec4(0.95f, 0.22f, 0.22f, 0.95f); // Red: cheating without Safe Mode
        } else if (cfg.runtime.cheatedThisAttempt) {
            dotColor = ImVec4(0.98f, 0.52f, 0.15f, 0.95f); // Orange: cheated earlier this attempt
        } else {
            dotColor = ImVec4(0.20f, 0.88f, 0.42f, 0.90f); // Green: legit
        }
        bgDraw->AddCircleFilled(ImVec2(14.0f, 14.0f), 5.5f, ImGui::GetColorU32(dotColor));
        bgDraw->AddCircle(ImVec2(14.0f, 14.0f), 5.5f, IM_COL32(0, 0, 0, 180), 0, 1.2f);
    }

    // 3. Status HUD (FPS / Noclip Accuracy / StartPos)
    bool showNoclipHud = cfg.runtime.inLevel && cfg.noclip && cfg.noclipAccuracy;
    bool showStartPosHud = cfg.runtime.inLevel && cfg.startPosSwitcher && cfg.runtime.startPosCount > 0;
    if (cfg.showFpsCounter || showNoclipHud || showStartPosHud) {
        ImGui::SetNextWindowPos(ImVec2(28.0f, 6.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.45f);
        ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoNav
            | ImGuiWindowFlags_NoMove;
        if (ImGui::Begin("##MegaLarpHUD", nullptr, hudFlags)) {
            if (cfg.showFpsCounter) {
                ImGui::Text("FPS: %.0f", cfg.runtime.currentFps);
            }
            if (showNoclipHud) {
                ImGui::Text("Accuracy: %.2f%% | Deaths: %d",
                    cfg.runtime.getNoclipAccuracy(),
                    cfg.runtime.noclipDeaths);
            }
            if (showStartPosHud) {
                if (ImGui::SmallButton("<")) {
                    cfg.runtime.pendingStartPosDelta -= 1;
                }
                ImGui::SameLine();
                ImGui::Text("StartPos: %d / %d",
                    cfg.runtime.currentStartPosIndex + 1,
                    cfg.runtime.startPosCount);
                ImGui::SameLine();
                if (ImGui::SmallButton(">")) {
                    cfg.runtime.pendingStartPosDelta += 1;
                }
            }
        }
        ImGui::End();
    }

    // 4. Optional floating toggle button (mobile / touch)
    if (cfg.showMobileButton) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 68.0f, 10.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.75f);
        ImGuiWindowFlags btnFlags = ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoNav;
        if (ImGui::Begin("##MegaLarpBtn", nullptr, btnFlags)) {
            if (ImGui::Button("ML", ImVec2(42.0f, 28.0f))) {
                toggle();
            }
        }
        ImGui::End();
    }
}

void Menu::renderFloatingWindows() {
    auto& cfg = Config::get();

    // Compact Top Bar (Search & Brand)
    ImGui::SetNextWindowPos(ImVec2(20.0f, 34.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420.0f, 0.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("MegaLarp v1.0", &m_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetNextItemWidth(220.0f);
        ImGui::InputTextWithHint("##search", "Search hacks...", m_searchBuf, sizeof(m_searchBuf));
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            m_searchBuf[0] = '\0';
        }
        ImGui::SameLine();
        ImGui::TextDisabled("[%s]", Keybinds::keyName(Keybinds::get().menuToggleKey()));
    }
    ImGui::End();

    // Floating Category Windows (Classic MegaHack layout)
    const float colW = 245.0f;
    const float startY = 104.0f;
    const float gap = 12.0f;

    ImGui::SetNextWindowPos(ImVec2(20.0f, startY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(colW, 430.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Gameplay")) {
        drawGameplaySection();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20.0f + (colW + gap) * 1, startY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(colW, 430.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Visual & FPS")) {
        drawVisualSection();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20.0f + (colW + gap) * 2, startY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(colW, 430.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Creator")) {
        drawCreatorSection();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20.0f + (colW + gap) * 3, startY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(colW, 430.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Global & QoL")) {
        drawGlobalSection();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20.0f + (colW + gap) * 4, startY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(260.0f, 430.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Settings & Keybinds")) {
        drawSettingsSection();
    }
    ImGui::End();
}

void Menu::renderTabbedWindow() {
    ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(560.0f, 480.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("MegaLarp v1.0", &m_open)) {
        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##search_tab", "Search hacks...", m_searchBuf, sizeof(m_searchBuf));
        ImGui::SameLine();
        if (ImGui::Button("Clear")) m_searchBuf[0] = '\0';

        ImGui::Separator();

        if (ImGui::BeginTabBar("##MegaLarpTabs")) {
            if (ImGui::BeginTabItem("Gameplay")) {
                drawGameplaySection();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Visual & FPS")) {
                drawVisualSection();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Creator")) {
                drawCreatorSection();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Global & QoL")) {
                drawGlobalSection();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings")) {
                drawSettingsSection();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void Menu::drawGameplaySection() {
    auto& cfg = Config::get();

    checkbox("Noclip", &cfg.noclip, "Prevents the player from dying on obstacles.", true);
    if (cfg.noclip) {
        ImGui::Indent(14.0f);
        checkbox("Noclip Red Tint", &cfg.noclipTintOnDeath, "Flashes red when hitting a hazard.");
        checkbox("Noclip Accuracy HUD", &cfg.noclipAccuracy, "Shows accuracy % and deaths in level.");
        ImGui::Unindent(14.0f);
    }

    checkbox("Safe Mode", &cfg.safeMode, "Prevents saving stars, orbs, and level progress.");
    checkbox("Auto Safe Mode", &cfg.autoSafeMode, "Automatically enables Safe Mode when cheats are active.");

    ImGui::Separator();

    checkbox("Speedhack", &cfg.speedhackEnabled, "Multiplies game simulation speed.", true);
    if (matchesSearch("Speedhack")) {
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##speed_val", &cfg.speedhackValue, 0.05f, 5.0f, "%.2fx")) {
            cfg.markCheatedIfNeeded();
            cfg.save();
        }
        checkbox("Speedhack Audio Pitch", &cfg.speedhackAudio, "Matches music pitch to speedhack value.");
    }

    ImGui::Separator();

    checkbox("Practice Music Sync", &cfg.practiceMusicSync, "Plays normal level music in Practice Mode.");
    checkbox("Auto Practice Mode", &cfg.autoPracticeMode, "Automatically enters Practice Mode on level start.");
    checkbox("StartPos Switcher", &cfg.startPosSwitcher, "Switch between StartPos objects (default Q / E).");
    checkbox("Instant Restart", &cfg.instantRestart, "Skips death delay and restarts immediately.");
    checkbox("Jump Hack", &cfg.jumpHack, "Allows jumping in mid-air.", true);
    checkbox("Instant Complete", &cfg.instantComplete, "Completes the level immediately upon entry.", true);
    checkbox("No Respawn Blink", &cfg.noRespawnBlink, "Disables the spawn flash/blink animation.");
    checkbox("No Death Effect", &cfg.noDeathEffect, "Disables player explosion particles on death.");
    checkbox("No Mirror Effect", &cfg.noMirrorEffect, "Disables mirror portals flipping the screen.", true);
}

void Menu::drawVisualSection() {
    auto& cfg = Config::get();

    if (checkbox("FPS Bypass", &cfg.fpsBypassEnabled, "Overrides the frame rate limit.")) {
        applyFpsBypassNow();
    }
    if (matchesSearch("FPS Bypass")) {
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##fps_val", &cfg.fpsBypassValue, 30.0f, 360.0f, "%.0f FPS")) {
            if (cfg.fpsBypassEnabled) applyFpsBypassNow();
            cfg.save();
        }
    }

    ImGui::Separator();

    checkbox("Show Hitboxes", &cfg.showHitboxes, "Renders object & player hitboxes while playing.", true);
    checkbox("Show Hitboxes on Death", &cfg.showHitboxesOnDeath, "Reveals hitboxes briefly when you die.");

    ImGui::Separator();

    checkbox("Hide Pause Menu", &cfg.hidePauseMenu, "Hides the pause menu overlay while paused.");
    checkbox("Hide Practice Buttons", &cfg.hidePracticeButtons, "Hides checkpoint +/- buttons in Practice Mode.");
    checkbox("Hide Attempts", &cfg.hideAttempts, "Hides the Attempt X label.");
    checkbox("No Wave Pulse", &cfg.noWavePulse, "Keeps wave trail width constant regardless of music.");
    checkbox("No Glow", &cfg.noGlow, "Disables glow on game objects.");
    checkbox("No Camera Shake", &cfg.noCameraShake, "Disables screen shake triggers.");

    ImGui::Separator();

    if (matchesSearch("Trail Mode")) {
        const char* trailModes[] = { "Normal", "Always On", "Always Off" };
        ImGui::TextUnformatted("Player Trail");
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::Combo("##trail_mode", &cfg.trailMode, trailModes, IM_ARRAYSIZE(trailModes))) {
            cfg.save();
        }
    }

    checkbox("Rainbow Icon", &cfg.rainbowIcon, "Smoothly cycles player icon colors.");
    if (cfg.rainbowIcon && matchesSearch("Rainbow")) {
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##rainbow_spd", &cfg.rainbowSpeed, 0.1f, 5.0f, "Speed %.1fx")) {
            cfg.save();
        }
    }
}

void Menu::drawCreatorSection() {
    auto& cfg = Config::get();

    checkbox("Copy Hack", &cfg.copyHack, "Allows copying any online level without a password.");
    checkbox("Verify Hack", &cfg.verifyHack, "Allows uploading levels without verifying them.");
    checkbox("Object Limit Bypass", &cfg.objectLimitBypass, "Removes the editor object limit warning/cap.");
    checkbox("Custom Object Bypass", &cfg.customObjectBypass, "Removes the custom object count limit.");
    checkbox("Slider Limit Bypass", &cfg.sliderLimitBypass, "Allows dragging editor sliders past min/max bounds.");
    checkbox("Level Edit", &cfg.levelEdit, "Enables the editor button on any level in Pause Menu.");
    checkbox("Default Song Bypass", &cfg.defaultSongBypass, "Allows selecting hidden/extra default songs in editor.");
}

void Menu::drawGlobalSection() {
    auto& cfg = Config::get();

    checkbox("Unlock All Icons", &cfg.unlockAllIcons, "Unlocks all icons, ships, balls, trails, and colors locally.");
    checkbox("No Transition", &cfg.noTransition, "Removes scene fade transitions for instant navigation.");
    checkbox("Fast Menu", &cfg.fastMenu, "Speeds up menu fade/popup animations.");
    checkbox("Confirm Exit", &cfg.confirmExit, "Asks for confirmation before exiting a level.");

    ImGui::Separator();

    checkbox("Cheat Indicator", &cfg.cheatIndicator, "Shows status dot in top-left (Green/Yellow/Red).");
    checkbox("Show FPS Counter", &cfg.showFpsCounter, "Displays live FPS overlay.");
    checkbox("Floating Mobile Button", &cfg.showMobileButton, "Shows a small draggable 'ML' button to open the menu.");
}

void Menu::drawSettingsSection() {
    auto& cfg = Config::get();
    auto& kb = Keybinds::get();

    ImGui::TextUnformatted("Appearance");
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::BeginCombo("##theme_combo", Theme::getPresetName(cfg.themePreset))) {
        for (int i = 0; i < Theme::PresetCount; ++i) {
            bool selected = (cfg.themePreset == i);
            if (ImGui::Selectable(Theme::getPresetName(i), selected)) {
                cfg.themePreset = i;
                m_themeDirty = true;
                cfg.save();
            }
            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::SliderFloat("UI Scale", &cfg.menuScale, 0.75f, 1.75f, "%.2f")) {
        m_themeDirty = true;
        cfg.save();
    }
    if (ImGui::SliderFloat("Opacity", &cfg.menuOpacity, 0.40f, 1.00f, "%.2f")) {
        m_themeDirty = true;
        cfg.save();
    }
    if (ImGui::Checkbox("Tabbed Window Mode", &cfg.tabbedMode)) {
        cfg.save();
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Keybinds (ESC to clear)");

    // Menu toggle keybind
    {
        bool listening = (kb.listeningId() == "__menu_toggle__");
        std::string btnLabel = listening ? "[Press Key...]" : std::string("[") + Keybinds::keyName(kb.menuToggleKey()) + "]";
        ImGui::TextUnformatted("Toggle Menu");
        ImGui::SameLine(145.0f);
        if (ImGui::Button((btnLabel + "##menu_toggle").c_str(), ImVec2(95.0f, 0.0f))) {
            kb.startListening("__menu_toggle__");
        }
    }

    for (auto& entry : kb.entries()) {
        bool listening = (kb.listeningId() == entry.id);
        std::string btnLabel = listening ? "[Press Key...]" : std::string("[") + Keybinds::keyName(entry.key) + "]";
        ImGui::TextUnformatted(entry.label.c_str());
        ImGui::SameLine(145.0f);
        if (ImGui::Button((btnLabel + "##kb_" + entry.id).c_str(), ImVec2(95.0f, 0.0f))) {
            kb.startListening(entry.id);
        }
    }

    ImGui::Separator();
    if (ImGui::Button("Reset All Settings", ImVec2(-1.0f, 0.0f))) {
        cfg.resetDefaults();
        m_themeDirty = true;
        applyFpsBypassNow();
    }
}

} // namespace megalarp
