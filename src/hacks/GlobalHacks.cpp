#include "../config/Config.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCTransitionFade.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;
using namespace megalarp;

class $modify(MLGameManager, GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (Config::get().unlockAllIcons) {
            return true;
        }
        return GameManager::isIconUnlocked(id, type);
    }

    bool isColorUnlocked(int id, UnlockType type) {
        if (Config::get().unlockAllIcons) {
            return true;
        }
        return GameManager::isColorUnlocked(id, type);
    }

    bool getGameVariable(char const* key) {
        auto& cfg = Config::get();
        if (key) {
            // 0066 = Disable high object alert / bypass editor limit warning
            if (cfg.objectLimitBypass && std::string_view(key) == "0066") {
                return true;
            }
        }
        return GameManager::getGameVariable(key);
    }
};

class $modify(MLTransitionFade, CCTransitionFade) {
    static CCTransitionFade* create(float duration, CCScene* scene) {
        auto& cfg = Config::get();
        if (cfg.noTransition) {
            duration = 0.0f;
        } else if (cfg.fastMenu) {
            duration *= 0.25f;
        }
        return CCTransitionFade::create(duration, scene);
    }

    static CCTransitionFade* create(float duration, CCScene* scene, ccColor3B const& color) {
        auto& cfg = Config::get();
        if (cfg.noTransition) {
            duration = 0.0f;
        } else if (cfg.fastMenu) {
            duration *= 0.25f;
        }
        return CCTransitionFade::create(duration, scene, color);
    }
};

class $modify(MLEditorUI, EditorUI) {
    void onNewCustomItem(CCObject* sender) {
        auto& cfg = Config::get();
        if (cfg.customObjectBypass && m_selectedObjects && m_selectedObjects->count() > 0) {
            // Allow saving large custom object groups
            EditorUI::onNewCustomItem(sender);
            return;
        }
        EditorUI::onNewCustomItem(sender);
    }
};
