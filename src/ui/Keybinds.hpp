#pragma once

#include <imgui.h>
#include <functional>
#include <string>
#include <vector>

namespace megalarp {

struct KeybindEntry {
    std::string id;
    std::string label;
    ImGuiKey key = ImGuiKey_None;
    std::function<void()> callback;
};

class Keybinds {
public:
    static Keybinds& get();

    void init();
    void load();
    void save() const;

    void registerBind(std::string id, std::string label, ImGuiKey defaultKey, std::function<void()> cb);
    void poll(bool menuOpen);

    std::vector<KeybindEntry>& entries() { return m_entries; }
    const std::string& listeningId() const { return m_listeningId; }
    void startListening(const std::string& id) { m_listeningId = id; }
    void cancelListening() { m_listeningId.clear(); }

    ImGuiKey menuToggleKey() const { return m_menuToggleKey; }
    void setMenuToggleKey(ImGuiKey key);

    static const char* keyName(ImGuiKey key);

private:
    ImGuiKey m_menuToggleKey = ImGuiKey_Tab;
    std::vector<KeybindEntry> m_entries;
    std::string m_listeningId;
    bool m_initialized = false;
};

} // namespace megalarp
