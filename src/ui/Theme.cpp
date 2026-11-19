#include "Theme.hpp"
#include <imgui.h>
#include <algorithm>

namespace megalarp {

const char* Theme::getPresetName(int index) {
    switch (static_cast<ThemePreset>(index)) {
        case ThemePreset::MegaLarpDark: return "MegaLarp Dark";
        case ThemePreset::CyberPurple:  return "Cyber Purple";
        case ThemePreset::ClassicGreen: return "Classic MH Green";
        case ThemePreset::CrimsonRed:   return "Crimson Red";
        default:                        return "MegaLarp Dark";
    }
}

void Theme::apply(ThemePreset preset, float opacity, float scale) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    opacity = std::clamp(opacity, 0.35f, 1.0f);
    scale = std::clamp(scale, 0.75f, 2.0f);

    // Geometry & spacing
    style.WindowPadding = ImVec2(10.0f, 8.0f);
    style.FramePadding = ImVec2(8.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.WindowRounding = 6.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 5.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.ScrollbarSize = 12.0f;

    ImGui::GetIO().FontGlobalScale = scale;

    // Accent palette per preset
    ImVec4 accent, accentHover, accentActive, bg, bgChild, headerBg;
    switch (preset) {
        case ThemePreset::CyberPurple:
            accent       = ImVec4(0.58f, 0.28f, 0.96f, 1.00f);
            accentHover  = ImVec4(0.68f, 0.40f, 1.00f, 1.00f);
            accentActive = ImVec4(0.46f, 0.18f, 0.85f, 1.00f);
            bg           = ImVec4(0.08f, 0.07f, 0.12f, opacity);
            bgChild      = ImVec4(0.11f, 0.09f, 0.16f, opacity);
            headerBg     = ImVec4(0.15f, 0.11f, 0.22f, 1.00f);
            break;
        case ThemePreset::ClassicGreen:
            accent       = ImVec4(0.16f, 0.78f, 0.42f, 1.00f);
            accentHover  = ImVec4(0.24f, 0.88f, 0.50f, 1.00f);
            accentActive = ImVec4(0.11f, 0.65f, 0.34f, 1.00f);
            bg           = ImVec4(0.07f, 0.09f, 0.08f, opacity);
            bgChild      = ImVec4(0.10f, 0.13f, 0.11f, opacity);
            headerBg     = ImVec4(0.12f, 0.20f, 0.15f, 1.00f);
            break;
        case ThemePreset::CrimsonRed:
            accent       = ImVec4(0.88f, 0.20f, 0.28f, 1.00f);
            accentHover  = ImVec4(0.96f, 0.30f, 0.38f, 1.00f);
            accentActive = ImVec4(0.74f, 0.13f, 0.20f, 1.00f);
            bg           = ImVec4(0.10f, 0.07f, 0.08f, opacity);
            bgChild      = ImVec4(0.14f, 0.09f, 0.10f, opacity);
            headerBg     = ImVec4(0.22f, 0.10f, 0.12f, 1.00f);
            break;
        case ThemePreset::MegaLarpDark:
        default:
            accent       = ImVec4(0.20f, 0.60f, 0.98f, 1.00f);
            accentHover  = ImVec4(0.32f, 0.70f, 1.00f, 1.00f);
            accentActive = ImVec4(0.14f, 0.48f, 0.86f, 1.00f);
            bg           = ImVec4(0.08f, 0.09f, 0.11f, opacity);
            bgChild      = ImVec4(0.11f, 0.12f, 0.15f, opacity);
            headerBg     = ImVec4(0.13f, 0.16f, 0.21f, 1.00f);
            break;
    }

    colors[ImGuiCol_Text]                 = ImVec4(0.94f, 0.95f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.52f, 0.55f, 0.62f, 1.00f);
    colors[ImGuiCol_WindowBg]             = bg;
    colors[ImGuiCol_ChildBg]              = bgChild;
    colors[ImGuiCol_PopupBg]              = ImVec4(bg.x, bg.y, bg.z, 0.97f);
    colors[ImGuiCol_Border]               = ImVec4(accent.x, accent.y, accent.z, 0.35f);
    colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg]              = ImVec4(0.16f, 0.18f, 0.22f, 0.90f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.22f, 0.25f, 0.30f, 0.95f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.26f, 0.30f, 0.36f, 1.00f);

    colors[ImGuiCol_TitleBg]              = headerBg;
    colors[ImGuiCol_TitleBgActive]        = accentActive;
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(headerBg.x, headerBg.y, headerBg.z, 0.75f);

    colors[ImGuiCol_MenuBarBg]            = headerBg;
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.06f, 0.07f, 0.09f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.30f, 0.33f, 0.40f, 0.85f);
    colors[ImGuiCol_ScrollbarGrabHovered] = accentHover;
    colors[ImGuiCol_ScrollbarGrabActive]  = accent;

    colors[ImGuiCol_CheckMark]            = accentHover;
    colors[ImGuiCol_SliderGrab]           = accent;
    colors[ImGuiCol_SliderGrabActive]     = accentHover;

    colors[ImGuiCol_Button]               = ImVec4(accent.x, accent.y, accent.z, 0.78f);
    colors[ImGuiCol_ButtonHovered]        = accentHover;
    colors[ImGuiCol_ButtonActive]         = accentActive;

    colors[ImGuiCol_Header]               = ImVec4(accent.x, accent.y, accent.z, 0.35f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(accent.x, accent.y, accent.z, 0.55f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(accent.x, accent.y, accent.z, 0.75f);

    colors[ImGuiCol_Separator]            = ImVec4(accent.x, accent.y, accent.z, 0.25f);
    colors[ImGuiCol_SeparatorHovered]     = accentHover;
    colors[ImGuiCol_SeparatorActive]      = accent;

    colors[ImGuiCol_Tab]                  = ImVec4(headerBg.x, headerBg.y, headerBg.z, 0.85f);
    colors[ImGuiCol_TabHovered]           = accentHover;
    colors[ImGuiCol_TabActive]            = accent;
    colors[ImGuiCol_TabUnfocused]         = headerBg;
    colors[ImGuiCol_TabUnfocusedActive]   = accentActive;
}

} // namespace megalarp
