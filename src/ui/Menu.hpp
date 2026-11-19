#pragma once

#include <string>

namespace megalarp {

class Menu {
public:
    static Menu& get();

    void setup();
    void render();

    bool isOpen() const { return m_open; }
    void setOpen(bool open) { m_open = open; }
    void toggle() { m_open = !m_open; }

private:
    void renderHudOverlays();
    void renderFloatingWindows();
    void renderTabbedWindow();

    void drawGameplaySection();
    void drawVisualSection();
    void drawCreatorSection();
    void drawGlobalSection();
    void drawSettingsSection();

    bool matchesSearch(const char* text) const;
    bool checkbox(const char* label, bool* value, const char* tooltip = nullptr, bool marksCheat = false);

    bool m_open = false;
    bool m_themeDirty = true;
    char m_searchBuf[128] = {};
};

} // namespace megalarp
