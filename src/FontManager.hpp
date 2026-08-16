#pragma once

#include <string_view>

class FontManager {
public:
    FontManager() = delete;
    ~FontManager() = delete;

    static auto AddIconFonts(std::string_view ttf_, float size_) -> void;
};
