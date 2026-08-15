#include "gui/utils/FontManager.hpp"
#include <vector>
#include <cstdlib>

namespace enma::gui::utils {

    Font FontManager::_mainFont{};

    // Helper to merge two fonts into one atlas, heavily inspired by ImGui's MergeMode
    auto FontManager::LoadMergedFont(
        const char* baseFontPath_, int32_t baseFontSize_, const int32_t* baseCodepoints_, int32_t baseCodepointCount_,
        const char* iconFontPath_, int32_t iconFontSize_, const int32_t* iconCodepoints_, int32_t iconCodepointCount_,
        int32_t iconOffsetY_
    ) -> Font 
    {
        int32_t baseDataSize = 0;
        int32_t iconDataSize = 0;
        unsigned char* baseData = LoadFileData(baseFontPath_, &baseDataSize);
        unsigned char* iconData = LoadFileData(iconFontPath_, &iconDataSize);

        if (!baseData || !iconData) [[unlikely]] {
            if (baseData) UnloadFileData(baseData);
            if (iconData) UnloadFileData(iconData);
            return GetFontDefault();
        }

        int32_t baseGlyphCount = 0;
        int32_t iconGlyphCount = 0;
        
        // Load CPU glyph infos (Requires FONT_DEFAULT type = 0)
        GlyphInfo* baseGlyphs = LoadFontData(baseData, baseDataSize, baseFontSize_, baseCodepoints_, baseCodepointCount_, 0, &baseGlyphCount);
        GlyphInfo* iconGlyphs = LoadFontData(iconData, iconDataSize, iconFontSize_, iconCodepoints_, iconCodepointCount_, 0, &iconGlyphCount);

        int32_t totalGlyphCount = baseGlyphCount + iconGlyphCount;
        
        // Zero-allocation constraint during hot path means this is fine (happens during initialization)
        auto* mergedGlyphs = static_cast<GlyphInfo*>(std::malloc(static_cast<std::size_t>(totalGlyphCount) * sizeof(GlyphInfo)));

        // Copy base glyphs
        for (int32_t i = 0; i < baseGlyphCount; ++i) {
            mergedGlyphs[i] = baseGlyphs[i];
        }

        // Copy icon glyphs with the MergeMode Y-offset applied
        for (int32_t i = 0; i < iconGlyphCount; ++i) {
            mergedGlyphs[baseGlyphCount + i] = iconGlyphs[i];
            mergedGlyphs[baseGlyphCount + i].offsetY += iconOffsetY_;
        }

        // Pack both sets of glyphs into a single atlas
        Rectangle* mergedRecs = nullptr;
        Image atlas = GenImageFontAtlas(mergedGlyphs, &mergedRecs, totalGlyphCount, baseFontSize_, 2, 0);

        Font font{};
        font.baseSize = baseFontSize_;
        font.glyphCount = totalGlyphCount;
        font.glyphPadding = 2;
        font.texture = LoadTextureFromImage(atlas);
        font.recs = mergedRecs;
        
        // Zero out the image pointers in mergedGlyphs so Raylib's UnloadFont doesn't double-free them,
        // since the original Image data is still owned by baseGlyphs and iconGlyphs
        for (int32_t i = 0; i < totalGlyphCount; ++i) {
            mergedGlyphs[i].image = Image{}; 
        }
        font.glyphs = mergedGlyphs; // Font assumes ownership of this pointer

        // Cleanup intermediate resources
        UnloadImage(atlas);
        UnloadFontData(baseGlyphs, baseGlyphCount);
        UnloadFontData(iconGlyphs, iconGlyphCount);
        UnloadFileData(baseData);
        UnloadFileData(iconData);

        return font;
    }

    auto FontManager::Initialize() -> void
    {
        // Generate an array of codepoints to load
        constexpr int32_t BaseAsciiCount = 95;
        constexpr int32_t SpecificIconCount = 5;
        
        int32_t baseCodepoints[BaseAsciiCount];
        for (int32_t i = 0; i < BaseAsciiCount; ++i) {
            baseCodepoints[i] = 32 + i;
        }
        
        constexpr auto Utf8ToCodepoint = [](const char* utf8) -> int32_t {
            return ((utf8[0] & 0x0F) << 12) | ((utf8[1] & 0x3F) << 6) | (utf8[2] & 0x3F);
        };
        
        int32_t iconCodepoints[SpecificIconCount];
        int32_t index = 0;
        iconCodepoints[index++] = Utf8ToCodepoint(ICON_MD_FILTER_ALT);
        iconCodepoints[index++] = Utf8ToCodepoint(ICON_MD_EXPAND_MORE);
        iconCodepoints[index++] = Utf8ToCodepoint(ICON_MD_CHECK_BOX_OUTLINE_BLANK);
        iconCodepoints[index++] = Utf8ToCodepoint(ICON_MD_CHECK_BOX);
        iconCodepoints[index++] = Utf8ToCodepoint(ICON_MD_SEARCH);
        
        // Merge the UbuntuMono Font as base, and Material Icons as icons
        // We load them at size 32 (double the render size) and apply Bilinear filtering below to make them silky smooth
        _mainFont = LoadMergedFont(
            "resources/fonts/UbuntuMono-R.ttf", 32, baseCodepoints, BaseAsciiCount,
            "resources/fonts/MaterialIcons-Regular.ttf", 32, iconCodepoints, index,
            0 // GlyphOffset.y
        );
        
        if (_mainFont.texture.id == 0) {
            _mainFont = GetFontDefault();
        } else {
            // This is the critical fix for pixelation! It allows smooth downscaling instead of nearest-neighbor blockiness
            SetTextureFilter(_mainFont.texture, TEXTURE_FILTER_BILINEAR);
        }
    }

    auto FontManager::Unload() -> void
    {
        // Don't unload if it's the default font
        if (_mainFont.texture.id != GetFontDefault().texture.id) {
            UnloadFont(_mainFont);
        }
    }

} // namespace enma::gui::utils
