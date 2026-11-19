#include "Keybinds.hpp"
#include "../config/Config.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace megalarp {

Keybinds& Keybinds::get() {
    static Keybinds instance;
    return instance;
}

const char* Keybinds::keyName(ImGuiKey key) {
    if (key == ImGuiKey_None) return "None";
    const char* name = ImGui::GetKeyName(key);
    return name ? name : "Unknown";
}

void Keybinds::setMenuToggleKey(ImGuiKey key) {
    m_menuToggleKey = key;
    Mod::get()->setSavedValue("keybind.menu_toggle", static_cast<int64_t>(key));
}

void Keybinds::registerBind(std::string id, std::string label, ImGuiKey defaultKey, std::function<void()> cb) {
    for (auto& e : m_entries) {
        if (e.id == id) {
            e.label = std::move(label);
            e.callback = std::move(cb);
            return;
        }
    }
    auto saved = static_cast<ImGuiKey>(
        Mod::get()->getSavedValue<int64_t>("keybind." + id, static_cast<int64_t>(defaultKey))
    );
    m_entries.push_back(KeybindEntry{
        std::move(id),
        std::move(label),
        saved,
        std::move(cb)
    });
}

void Keybinds::init() {
    if (m_initialized) return;
    m_initialized = true;

    m_menuToggleKey = static_cast<ImGuiKey>(
        Mod::get()->getSavedValue<int64_t>("keybind.menu_toggle", static_cast<int64_t>(ImGuiKey_Tab))
    );

    auto& cfg = Config::get();

    registerBind("noclip", "Toggle Noclip", ImGuiKey_None, [&cfg] {
        cfg.noclip = !cfg.noclip;
        cfg.markCheatedIfNeeded();
        cfg.save();
    });
    registerBind("speedhack", "Toggle Speedhack", ImGuiKey_None, [&cfg] {
        cfg.speedhackEnabled = !cfg.speedhackEnabled;
        cfg.markCheatedIfNeeded();
        cfg.save();
    });
    registerBind("hitboxes", "Toggle Show Hitboxes", ImGuiKey_None, [&cfg] {
        cfg.showHitboxes = !cfg.showHitboxes;
        cfg.markCheatedIfNeeded();
        cfg.save();
    });
    registerBind("safemode", "Toggle Safe Mode", ImGuiKey_None, [&cfg] {
        cfg.safeMode = !cfg.safeMode;
        cfg.save();
    });
    registerBind("startpos_prev", "StartPos Previous", ImGuiKey_Q, [&cfg] {
        if (cfg.startPosSwitcher && cfg.runtime.inLevel) {
            cfg.runtime.pendingStartPosDelta -= 1;
        }
    });
    registerBind("startpos_next", "StartPos Next", ImGuiKey_E, [&cfg] {
        if (cfg.startPosSwitcher && cfg.runtime.inLevel) {
            cfg.runtime.pendingStartPosDelta += 1;
        }
    });
}

void Keybinds::load() {
    m_menuToggleKey = static_cast<ImGuiKey>(
        Mod::get()->getSavedValue<int64_t>("keybind.menu_toggle", static_cast<int64_t>(ImGuiKey_Tab))
    );
    for (auto& e : m_entries) {
        e.key = static_cast<ImGuiKey>(
            Mod::get()->getSavedValue<int64_t>("keybind." + e.id, static_cast<int64_t>(e.key))
        );
    }
}

void Keybinds::save() const {
    auto* mod = Mod::get();
    mod->setSavedValue("keybind.menu_toggle", static_cast<int64_t>(m_menuToggleKey));
    for (const auto& e : m_entries) {
        mod->setSavedValue("keybind." + e.id, static_cast<int64_t>(e.key));
    }
}

void Keybinds::poll(bool menuOpen) {
    // Handle interactive rebinding first
    if (!m_listeningId.empty()) {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
            if (m_listeningId == "__menu_toggle__") {
                setMenuToggleKey(ImGuiKey_Tab);
            } else {
                for (auto& e : m_entries) {
                    if (e.id == m_listeningId) {
                        e.key = ImGuiKey_None;
                        Mod::get()->setSavedValue("keybind." + e.id, static_cast<int64_t>(e.key));
                        break;
                    }
                }
            }
            m_listeningId.clear();
            return;
        }

        for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k) {
            auto key = static_cast<ImGuiKey>(k);
            // Ignore mouse buttons when binding keyboard shortcuts
            if (key >= ImGuiKey_MouseLeft && key <= ImGuiKey_MouseWheelY) continue;
            if (ImGui::IsKeyPressed(key, false)) {
                if (m_listeningId == "__menu_toggle__") {
                    setMenuToggleKey(key);
                } else {
                    for (auto& e : m_entries) {
                        if (e.id == m_listeningId) {
                            e.key = key;
                            Mod::get()->setSavedValue("keybind." + e.id, static_cast<int64_t>(e.key));
                            break;
                        }
                    }
                }
                m_listeningId.clear();
                return;
            }
        }
        return;
    }

    // Do not fire hack hotkeys while typing in an ImGui text input
    if (menuOpen && ImGui::GetIO().WantTextInput) {
        return;
    }

    for (auto& e : m_entries) {
        if (e.key != ImGuiKey_None && ImGui::IsKeyPressed(e.key, false)) {
            if (e.callback) e.callback();
        }
    }
}

} // namespace megalarp
