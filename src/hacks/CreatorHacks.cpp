#include "../config/Config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>
#include <Geode/modify/LevelTools.hpp>

using namespace geode::prelude;
using namespace megalarp;

class $modify(MLLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (Config::get().copyHack && level) {
            // Password value 1 enables direct password-free copying in GD
            if (level->m_password == 0) {
                level->m_password = 1;
            }
        }
        return LevelInfoLayer::init(level, challenge);
    }
};

class $modify(MLEditLevelLayer, EditLevelLayer) {
    void onShare(CCObject* sender) {
        if (Config::get().verifyHack && m_level) {
            m_level->m_isVerified = true;
            m_level->m_isVerifiedRaw = true;
        }
        EditLevelLayer::onShare(sender);
    }
};

class $modify(MLSliderTouchLogic, SliderTouchLogic) {
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (!Config::get().sliderLimitBypass) {
            SliderTouchLogic::ccTouchMoved(touch, event);
            return;
        }

        // Allow dragging slider thumb beyond standard [0, 1] bounds
        if (m_thumb && m_slider) {
            auto localPos = this->convertTouchToNodeSpace(touch);
            float halfLen = m_length * 0.5f;
            float x = localPos.x;
            m_thumb->setPosition(ccp(x, m_thumb->getPositionY()));
            if (m_length > 0.0f) {
                float val = (x + halfLen) / m_length;
                m_thumb->setValue(val);
            }
            m_slider->updateBar();
            return;
        }
        SliderTouchLogic::ccTouchMoved(touch, event);
    }
};

class $modify(MLLevelTools, LevelTools) {
    static gd::string getAudioTitle(int audioID) {
        if (Config::get().defaultSongBypass && audioID < 0) {
            return "Custom / Hidden Track";
        }
        return LevelTools::getAudioTitle(audioID);
    }
};
