#include "../config/Config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace geode::prelude;
using namespace megalarp;

class $modify(MLPlayLayer, PlayLayer) {
    struct Fields {
        std::vector<StartPosObject*> startPositions;
        bool diedThisFrame = false;
        bool wasInHazard = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        auto& cfg = Config::get();
        cfg.runtime.inLevel = true;
        cfg.runtime.resetAttempt();
        cfg.runtime.currentStartPosIndex = -1;
        cfg.runtime.pendingStartPosDelta = 0;

        // Collect all StartPos objects sorted by X coordinate
        m_fields->startPositions.clear();
        if (m_objects) {
            for (auto* obj : CCArrayExt<GameObject*>(m_objects)) {
                if (obj && obj->m_objectID == 31) { // StartPos object ID = 31
                    if (auto* sp = typeinfo_cast<StartPosObject*>(obj)) {
                        m_fields->startPositions.push_back(sp);
                    }
                }
            }
            std::sort(m_fields->startPositions.begin(), m_fields->startPositions.end(),
                [](StartPosObject* a, StartPosObject* b) {
                    return a->getPositionX() < b->getPositionX();
                });
        }
        cfg.runtime.startPosCount = static_cast<int>(m_fields->startPositions.size());

        if (cfg.autoPracticeMode && !m_isPracticeMode) {
            this->togglePracticeMode(true);
        }

        if (cfg.instantComplete) {
            cfg.markCheatedIfNeeded();
            this->levelComplete();
        }

        return true;
    }

    void onQuit() {
        auto& cfg = Config::get();
        cfg.runtime.inLevel = false;
        cfg.runtime.resetAttempt();
        cfg.runtime.startPosCount = 0;
        cfg.runtime.currentStartPosIndex = -1;
        PlayLayer::onQuit();
    }

    void resetLevel() {
        auto& cfg = Config::get();
        cfg.runtime.resetAttempt();
        m_fields->diedThisFrame = false;
        m_fields->wasInHazard = false;

        PlayLayer::resetLevel();

        cfg.markCheatedIfNeeded();

        if (cfg.instantComplete) {
            this->levelComplete();
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        // Always allow GD 2.2's internal anticheat spike verification
        if (obj == m_anticheatSpike) {
            PlayLayer::destroyPlayer(player, obj);
            return;
        }

        auto& cfg = Config::get();
        if (cfg.noclip) {
            m_fields->diedThisFrame = true;
            if (!m_fields->wasInHazard) {
                cfg.runtime.noclipDeaths += 1;
                m_fields->wasInHazard = true;
            }
            cfg.runtime.noclipTintAlpha = 1.0f;
            cfg.runtime.cheatedThisAttempt = true;
            return;
        }

        cfg.runtime.playerDead = true;
        PlayLayer::destroyPlayer(player, obj);

        if (cfg.instantRestart) {
            this->resetLevel();
        }
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto& cfg = Config::get();

        // Update noclip accuracy counters
        if (cfg.noclip && m_player1 && !m_player1->m_isDead) {
            cfg.runtime.noclipTotalFrames += 1;
            if (!m_fields->diedThisFrame) {
                cfg.runtime.noclipSafeFrames += 1;
                m_fields->wasInHazard = false;
            }
            m_fields->diedThisFrame = false;
        }

        // Hitboxes visibility (always on, or on death)
        if (m_debugDrawNode) {
            bool wantHitboxes = cfg.showHitboxes
                || (cfg.showHitboxesOnDeath && cfg.runtime.playerDead);
            if (wantHitboxes) {
                m_isDebugDrawEnabled = true;
                m_debugDrawNode->setVisible(true);
                this->updateDebugDraw();
            } else if (!m_isPracticeMode) {
                m_debugDrawNode->setVisible(false);
            }
        }

        // StartPos Switcher
        if (cfg.startPosSwitcher && cfg.runtime.pendingStartPosDelta != 0 && !m_fields->startPositions.empty()) {
            int count = static_cast<int>(m_fields->startPositions.size());
            int idx = cfg.runtime.currentStartPosIndex + cfg.runtime.pendingStartPosDelta;
            cfg.runtime.pendingStartPosDelta = 0;

            // Wrap around [-1, count - 1], where -1 = normal start of level
            if (idx >= count) idx = -1;
            if (idx < -1) idx = count - 1;
            cfg.runtime.currentStartPosIndex = idx;

            if (idx == -1) {
                this->setStartPosObject(nullptr);
            } else {
                this->setStartPosObject(m_fields->startPositions[idx]);
            }
            this->resetLevel();
            this->startMusic();
        }
    }

    void levelComplete() {
        auto& cfg = Config::get();
        if (cfg.isSafeModeEffective()) {
            bool origTestMode = m_isTestMode;
            m_isTestMode = true;
            PlayLayer::levelComplete();
            m_isTestMode = origTestMode;
            return;
        }
        PlayLayer::levelComplete();
    }

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        if (Config::get().isSafeModeEffective()) {
            return;
        }
        PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
    }

    void startMusic() {
        auto& cfg = Config::get();
        if (cfg.practiceMusicSync && m_isPracticeMode) {
            bool origPractice = m_isPracticeMode;
            m_isPracticeMode = false;
            PlayLayer::startMusic();
            m_isPracticeMode = origPractice;
            return;
        }
        PlayLayer::startMusic();
    }
};

class $modify(MLBaseGameLayer, GJBaseGameLayer) {
    void toggleFlipped(bool flipped, bool noEffects) {
        if (Config::get().noMirrorEffect) {
            return;
        }
        GJBaseGameLayer::toggleFlipped(flipped, noEffects);
    }

    void shakeCamera(float duration, float strength, float interval) {
        if (Config::get().noCameraShake) {
            return;
        }
        GJBaseGameLayer::shakeCamera(duration, strength, interval);
    }
};

class $modify(MLPlayerObject, PlayerObject) {
    void update(float dt) {
        if (Config::get().jumpHack) {
            m_isOnGround = true;
        }
        PlayerObject::update(dt);
    }

    void playDeathEffect() {
        if (Config::get().noDeathEffect) {
            return;
        }
        PlayerObject::playDeathEffect();
    }

    void playSpawnEffect() {
        if (Config::get().noRespawnBlink) {
            return;
        }
        PlayerObject::playSpawnEffect();
    }
};

class $modify(MLScheduler, CCScheduler) {
    void update(float dt) {
        auto& cfg = Config::get();
        float speed = 1.0f;
        if (cfg.speedhackEnabled) {
            speed = std::clamp(cfg.speedhackValue, 0.05f, 10.0f);
            dt *= speed;
        }

        if (auto* fmod = FMODAudioEngine::sharedEngine()) {
            if (fmod->m_backgroundMusicChannel) {
                float targetPitch = (cfg.speedhackEnabled && cfg.speedhackAudio) ? speed : 1.0f;
                fmod->m_backgroundMusicChannel->setPitch(targetPitch);
            }
        }

        CCScheduler::update(dt);
    }
};
