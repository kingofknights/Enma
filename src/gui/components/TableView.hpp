#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <array>
#include <cstring>
#include "gui/utils/Theme.hpp"
#include "gui/utils/FontManager.hpp"
#include "raylib.h"

namespace enma::gui::components {

    // Pre-allocate fixed limits to strictly abide by zero-allocation hot-path rules
    constexpr uint32_t MaxColumns = 16;
    constexpr uint32_t MaxFilterLength = 32;
    constexpr uint32_t MaxUniqueFilters = 32; // Limit to 32 checkboxes per column
    constexpr uint32_t MaxCellBufferSize = 255;
    constexpr int32_t  CellFontSize = 18;

    struct FilterItem {
        std::array<char, MaxFilterLength> _text{};
        uint32_t _len{0};
        bool _checked{true};
    };

    struct TableColumn {
        std::array<FilterItem, MaxUniqueFilters> _uniqueValues{}; // ~1280 bytes
        std::array<char, MaxFilterLength> _searchBuffer{};        // 32 bytes
        std::string_view _name;                                   // 16 bytes
        float _width{120.0f};                                     // 4 bytes
        uint32_t _searchLen{0};                                   // 4 bytes
        uint32_t _uniqueCount{0};                                 // 4 bytes
        bool _filterInitialized{false};                           // 1 byte
    };

    class TableView {
    public:
        // Constructor runs on setup: safe to initialize structure
        explicit TableView(std::span<const std::string_view> columnNames_);
        
        // Destructor
        ~TableView() = default;

        // Hot Path UI drawing loop - strictly template-based data provider to prevent std::function allocation overhead
        // The dataProvider_ must be a callable: auto(uint32_t row_, uint32_t col_) -> std::string_view
        template <typename DataProviderT>
        auto UpdateAndDraw(Rectangle bounds_, uint32_t totalRows_, DataProviderT&& dataProvider_) -> void 
        {
            HandleInput(bounds_);
            
            // Auto-scan unique values if a new dropdown was opened
            if (_state._activeDropdownColumn >= 0 && _state._activeDropdownColumn != _state._lastDropdownColumn) {
                InitializeColumnFilter(_state._activeDropdownColumn, totalRows_, dataProvider_);
                _state._lastDropdownColumn = _state._activeDropdownColumn;
            }

            DrawHeader(bounds_);
            DrawRows(bounds_, totalRows_, std::forward<DataProviderT>(dataProvider_));
            DrawDropdown(bounds_);
        }

    private:
        struct PrivateData {
            std::array<TableColumn, MaxColumns> _columns{};
            uint32_t _columnCount{0};
            float _rowHeight{28.0f};
            float _headerHeight{32.0f};
            float _scrollOffset{0.0f};
            int32_t _activeDropdownColumn{-1};
            int32_t _lastDropdownColumn{-1};
        };

        PrivateData _state{};

        // Internal rendering functions
        auto DrawHeader(Rectangle bounds_) -> void;
        auto HandleInput(Rectangle bounds_) -> void;
        auto DrawDropdown(Rectangle bounds_) -> void;
        
        template <typename DataProviderT>
        auto InitializeColumnFilter(uint32_t colIndex_, uint32_t totalRows_, DataProviderT&& dataProvider_) -> void 
        {
            auto& col = _state._columns[colIndex_];
            if (col._filterInitialized) return;

            col._uniqueCount = 0;
            for (uint32_t r = 0; r < totalRows_; ++r) {
                std::string_view cellText = dataProvider_(r, colIndex_);
                
                // Check if already in unique values
                bool found = false;
                for (uint32_t u = 0; u < col._uniqueCount; ++u) {
                    std::string_view existing(col._uniqueValues[u]._text.data(), col._uniqueValues[u]._len);
                    if (existing == cellText) {
                        found = true;
                        break;
                    }
                }

                if (!found && col._uniqueCount < MaxUniqueFilters) {
                    uint32_t copyLen = static_cast<uint32_t>(cellText.size());
                    if (copyLen > MaxFilterLength) [[unlikely]] {
                        copyLen = MaxFilterLength;
                    }
                    
                    std::memcpy(col._uniqueValues[col._uniqueCount]._text.data(), cellText.data(), copyLen);
                    col._uniqueValues[col._uniqueCount]._len = copyLen;
                    col._uniqueValues[col._uniqueCount]._checked = true;
                    col._uniqueCount++;
                }
            }
            col._filterInitialized = true;
        }

        // The template implementation must remain in the header
        template <typename DataProviderT>
        auto DrawRows(Rectangle bounds_, uint32_t totalRows_, DataProviderT&& dataProvider_) -> void 
        {
            float currentY = bounds_.y + _state._headerHeight;
            float availableHeight = bounds_.height - _state._headerHeight;
            uint32_t visibleRows = static_cast<uint32_t>(availableHeight / _state._rowHeight);

            uint32_t renderedRows = 0;
            
            for (uint32_t r = 0; r < totalRows_; ++r) {
                // Check if this row passes all active column filters
                bool passesFilter = true;
                for (uint32_t c = 0; c < _state._columnCount; ++c) {
                    auto& col = _state._columns[c];
                    if (!col._filterInitialized) continue;

                    std::string_view cellText = dataProvider_(r, c);
                    bool matchedAnyChecked = false;
                    bool existsInFilterList = false;

                    for (uint32_t u = 0; u < col._uniqueCount; ++u) {
                        std::string_view filterText(col._uniqueValues[u]._text.data(), col._uniqueValues[u]._len);
                        if (cellText == filterText) {
                            existsInFilterList = true;
                            if (col._uniqueValues[u]._checked) {
                                matchedAnyChecked = true;
                            }
                            break;
                        }
                    }

                    // If it's in our unique list but unchecked, hide it.
                    if (existsInFilterList && !matchedAnyChecked) {
                        passesFilter = false;
                        break;
                    }
                }

                if (!passesFilter) {
                    continue; // Skip rendering
                }

                float currentX = bounds_.x;
                
                // Draw row background alternating colors for readability using Catppuccin
                Color bgColor = (renderedRows % 2 == 0) ? theme::ThemeManager::Get()._rowEven : theme::ThemeManager::Get()._rowOdd;
                DrawRectangle(static_cast<int32_t>(currentX), static_cast<int32_t>(currentY), static_cast<int32_t>(bounds_.width), static_cast<int32_t>(_state._rowHeight), bgColor);

                // Draw each column cell
                for (uint32_t c = 0; c < _state._columnCount; ++c) {
                    std::string_view cellText = dataProvider_(r, c);
                    
                    thread_local std::array<char, MaxCellBufferSize + 1> textBuffer{};
                    uint32_t copyLen = static_cast<uint32_t>(cellText.size());
                    
                    if (copyLen > MaxCellBufferSize) [[unlikely]] {
                        copyLen = MaxCellBufferSize;
                    }

                    std::memcpy(textBuffer.data(), cellText.data(), copyLen);
                    textBuffer[copyLen] = '\0';

                    Vector2 cellSize = MeasureTextEx(utils::FontManager::GetFont(), textBuffer.data(), static_cast<float>(CellFontSize), 0.0f);
                    Vector2 cellPos = { currentX + 5.0f, currentY + (_state._rowHeight - cellSize.y) / 2.0f };
                    DrawTextEx(utils::FontManager::GetFont(), textBuffer.data(), cellPos, static_cast<float>(CellFontSize), 0.0f, theme::ThemeManager::Get()._textPrimary);
                    
                    currentX += _state._columns[c]._width;
                }
                
                currentY += _state._rowHeight;
                renderedRows++;

                if (renderedRows >= visibleRows) {
                    break;
                }
            }
        }
    };

} // namespace enma::gui::components
