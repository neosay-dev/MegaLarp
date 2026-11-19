#include "../config/Config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameObject.hpp>

#include <cmath>

using namespace geode::prelude;
using namespace megalarp;

namespace {

ccColor3B hsvToRgb(float h, float s, float v) {
    h = std::fmod(h, 1.0f);
    if (h < 0.0f) h += 1.0f;

    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h * 6.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r = 0.0f, g = 0.0f, b = 0.0f;
    int sector = static_cast<int>(h * 6.0f) % 6;
    switch (sector) {
        case 0: r = c; g = x; b = 0; break;
        case 1: r = x; g = c; b = 0; break;
        case 2: r = 0; g = c; b = x; break;
        case 3: r = 0; g = x; b = c; break;
        case 4: r = x; g = 0; b = c; break;
        case 5: r = c; g = 0; b = x; break;
    }

    return ccColor3B{
        static_cast<GLubyte>((r + m) * 255.0f),
        static_cast<GLubyte>((g + m) * 255.0f),
        static_cast<GLubyte>((b + m) * 255.0f)
    };
}

} // namespace

class $modify(MLPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto& cfg = Config::get();
        if (cfg.hidePauseMenu) {
            this->setVisible(false);
        }

        // Level Edit: allow opening the level editor from Pause Menu on any level
        if (cfg.levelEdit) {
            if (auto* menu = this->getChildByID("center-button-menu")) {
                if (!menu->getChildByID("edit-button")) {
                    auto* spr = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
                    if (spr) {
                        spr->setScale(0.75f);
                        auto* btn = CCMenuItemSpriteExtra::create(
                            spr, this, menu_selector(PauseLayer::onEdit)
                        );
                        btn->setID("edit-button");
                        menu->addChild(btn);
                        menu->updateLayout();
                    }
                }
            }
        }
    }

    void onQuit(CCObject* sender) {
        if (Config::get().confirmExit) {
            geode::createQuickPopup(
                "MegaLarp",
                "Are you sure you want to <cr>exit</c> this level?",
                "Cancel",
                "Exit",
                [this, sender](auto*, bool confirmed) {
                    if (confirmed) {
                        PauseLayer::onQuit(sender);
                    }
                }
            );
            return;
        }
        PauseLayer::onQuit(sender);
    }
};

class $modify(MLUILayer, UILayer) {
    bool init(GJBaseGameLayer* layer) {
        if (!UILayer::init(layer)) return false;
        if (Config::get().hidePracticeButtons && m_checkpointMenu) {
            m_checkpointMenu->setVisible(false);
        }
        return true;
    }
};

class $modify(MLVisualPlayLayer, PlayLayer) {
    void updateAttempts() {
        PlayLayer::updateAttempts();
        if (Config::get().hideAttempts && m_attemptLabel) {
            m_attemptLabel->setVisible(false);
        }
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto& cfg = Config::get();
        if (cfg.rainbowIcon) {
            cfg.runtime.rainbowHue = std::fmod(
                cfg.runtime.rainbowHue + dt * cfg.rainbowSpeed * 0.35f,
                1.0f
            );
            auto col1 = hsvToRgb(cfg.runtime.rainbowHue, 0.85f, 1.0f);
            auto col2 = hsvToRgb(cfg.runtime.rainbowHue + 0.5f, 0.85f, 1.0f);

            if (m_player1) {
                m_player1->setColor(col1);
                m_player1->setSecondColor(col2);
            }
            if (m_player2) {
                m_player2->setColor(col2);
                m_player2->setSecondColor(col1);
            }
        }
    }
};

class $modify(MLHardStreak, HardStreak) {
    void updateStroke(float dt) {
        if (Config::get().noWavePulse) {
            m_pulseSize = 1.0f;
        }
        HardStreak::updateStroke(dt);
    }
};

class $modify(MLVisualPlayerObject, PlayerObject) {
    void activateStreak() {
        auto mode = static_cast<TrailMode>(Config::get().trailMode);
        if (mode == TrailMode::AlwaysOff) {
            return;
        }
        PlayerObject::activateStreak();
    }

    void deactivateStreak(bool stop) {
        auto mode = static_cast<TrailMode>(Config::get().trailMode);
        if (mode == TrailMode::AlwaysOn) {
            return;
        }
        PlayerObject::deactivateStreak(stop);
    }
};

class $modify(MLGameObject, GameObject) {
    void addGlow(gd::string frame) {
        if (Config::get().noGlow) {
            return;
        }
        GameObject::addGlow(frame);
    }
};
