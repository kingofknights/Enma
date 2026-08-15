#pragma once

#include "raylib.h"
#include <cstdint>

namespace enma::gui::theme {

    enum class ThemeFlavor {
        CatppuccinLatte,
        CatppuccinFrappe,
        CatppuccinMacchiato,
        CatppuccinMocha
    };

    struct ThemeColors {
        Color _background;       // App background
        Color _surface;          // Panels, Popups
        Color _surfaceHighlight; // Hover states, selected items
        Color _border;           // Separators, outlines
        
        Color _textPrimary;      // Main text
        Color _textSecondary;    // Dimmer text, placeholders
        
        Color _accent;           // Primary brand/active color
        Color _success;          // Positive (e.g., BUY)
        Color _danger;           // Negative (e.g., SELL)
        Color _warning;          // Alerts
        
        // Table specific
        Color _rowEven;
        Color _rowOdd;
        Color _headerBg;
    };

    class ThemeManager {
    public:
        // Delete constructors to enforce static-only manager
        ThemeManager() = delete;

        // Initialize and Set Theme
        static auto SetTheme(ThemeFlavor flavor_) -> void;
        
        // Zero-allocation inline getter for the hot path
        [[nodiscard]] static inline auto Get() -> const ThemeColors& {
            return _activeColors;
        }

    private:
        static ThemeColors _activeColors;

        // Hardcoded zero-allocation flavor loaders
        [[nodiscard]] static auto LoadCatppuccinLatte() -> ThemeColors;
        [[nodiscard]] static auto LoadCatppuccinFrappe() -> ThemeColors;
        [[nodiscard]] static auto LoadCatppuccinMacchiato() -> ThemeColors;
        [[nodiscard]] static auto LoadCatppuccinMocha() -> ThemeColors;
        
        // Helper to cleanly map Hex to Raylib Color
        [[nodiscard]] static constexpr auto Hex(uint32_t hex_) -> Color {
            return {
                static_cast<unsigned char>((hex_ >> 16) & 0xFF),
                static_cast<unsigned char>((hex_ >> 8) & 0xFF),
                static_cast<unsigned char>(hex_ & 0xFF),
                255
            };
        }
    };

} // namespace enma::gui::theme
