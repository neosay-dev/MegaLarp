#pragma once

#include "../config/Config.hpp"

namespace megalarp {

class Theme {
public:
    static void apply(ThemePreset preset, float opacity = 0.94f, float scale = 1.0f);
    static const char* getPresetName(int index);
    static constexpr int PresetCount = 4;
};

} // namespace megalarp
