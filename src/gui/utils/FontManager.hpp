#pragma once

#include "raylib.h"
#include <cstdint>

#include "IconsMaterialDesign.h"

namespace enma::gui::utils {

    class FontManager {
    public:
        FontManager() = delete;

        static auto Initialize() -> void;
        static auto Unload() -> void;

        static auto LoadMergedFont(
            const char* baseFontPath_, int32_t baseFontSize_, const int32_t* baseCodepoints_, int32_t baseCodepointCount_,
            const char* iconFontPath_, int32_t iconFontSize_, const int32_t* iconCodepoints_, int32_t iconCodepointCount_,
            int32_t iconOffsetY_
        ) -> Font;

        [[nodiscard]] static inline auto GetFont() -> const Font& {
            return _mainFont;
        }

    private:
        static Font _mainFont;
    };

} // namespace enma::gui::utils
