#include "gui/utils/Theme.hpp"

namespace enma::gui::theme {

    // Define the static member
    ThemeColors ThemeManager::_activeColors = ThemeManager::LoadCatppuccinMocha();

    auto ThemeManager::SetTheme(ThemeFlavor flavor_) -> void 
    {
        switch (flavor_) {
            case ThemeFlavor::CatppuccinLatte:
                _activeColors = LoadCatppuccinLatte();
                break;
            case ThemeFlavor::CatppuccinFrappe:
                _activeColors = LoadCatppuccinFrappe();
                break;
            case ThemeFlavor::CatppuccinMacchiato:
                _activeColors = LoadCatppuccinMacchiato();
                break;
            case ThemeFlavor::CatppuccinMocha:
                _activeColors = LoadCatppuccinMocha();
                break;
        }
    }

    auto ThemeManager::LoadCatppuccinLatte() -> ThemeColors 
    {
        return {
            ._background       = Hex(0xeff1f5), // Base
            ._surface          = Hex(0xe6e9ef), // Surface0
            ._surfaceHighlight = Hex(0xccd0da), // Surface2
            ._border           = Hex(0x9ca0b0), // Overlay0
            ._textPrimary      = Hex(0x4c4f69), // Text
            ._textSecondary    = Hex(0x7c7f93), // Subtext0
            ._accent           = Hex(0x1e66f5), // Blue
            ._success          = Hex(0x40a02b), // Green
            ._danger           = Hex(0xd20f39), // Red
            ._warning          = Hex(0xdf8e1d), // Yellow
            ._rowEven          = Hex(0xeff1f5), // Base
            ._rowOdd           = Hex(0xe6e9ef), // Surface0
            ._headerBg         = Hex(0xccd0da)  // Surface2
        };
    }

    auto ThemeManager::LoadCatppuccinFrappe() -> ThemeColors 
    {
        return {
            ._background       = Hex(0x303446),
            ._surface          = Hex(0x414559),
            ._surfaceHighlight = Hex(0x626880),
            ._border           = Hex(0x737994),
            ._textPrimary      = Hex(0xc6d0f5),
            ._textSecondary    = Hex(0xa5adce),
            ._accent           = Hex(0x8caaee),
            ._success          = Hex(0xa6d189),
            ._danger           = Hex(0xe78284),
            ._warning          = Hex(0xe5c890),
            ._rowEven          = Hex(0x303446),
            ._rowOdd           = Hex(0x292c3c), // Mantle
            ._headerBg         = Hex(0x414559)
        };
    }

    auto ThemeManager::LoadCatppuccinMacchiato() -> ThemeColors 
    {
        return {
            ._background       = Hex(0x24273a),
            ._surface          = Hex(0x363a4f),
            ._surfaceHighlight = Hex(0x5b6078),
            ._border           = Hex(0x6e738d),
            ._textPrimary      = Hex(0xcad3f5),
            ._textSecondary    = Hex(0xa5adcb),
            ._accent           = Hex(0x8aadf4),
            ._success          = Hex(0xa6da95),
            ._danger           = Hex(0xed8796),
            ._warning          = Hex(0xeed49f),
            ._rowEven          = Hex(0x24273a),
            ._rowOdd           = Hex(0x1e2030), // Mantle
            ._headerBg         = Hex(0x363a4f)
        };
    }

    auto ThemeManager::LoadCatppuccinMocha() -> ThemeColors 
    {
        return {
            ._background       = Hex(0x1e1e2e),
            ._surface          = Hex(0x313244),
            ._surfaceHighlight = Hex(0x585b70),
            ._border           = Hex(0x6c7086),
            ._textPrimary      = Hex(0xcdd6f4),
            ._textSecondary    = Hex(0xa6adc8),
            ._accent           = Hex(0x89b4fa),
            ._success          = Hex(0xa6e3a1),
            ._danger           = Hex(0xf38ba8),
            ._warning          = Hex(0xf9e2af),
            ._rowEven          = Hex(0x1e1e2e),
            ._rowOdd           = Hex(0x181825), // Mantle
            ._headerBg         = Hex(0x313244)
        };
    }

} // namespace enma::gui::theme
