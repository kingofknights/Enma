#include "gui/components/TableView.hpp"
#include "gui/utils/Theme.hpp"
#include "gui/utils/FontManager.hpp"

namespace enma::gui::components {

TableView::TableView(std::span<const std::string_view> columnNames_) 
{
    _state._columnCount = (columnNames_.size() < MaxColumns) ? static_cast<uint32_t>(columnNames_.size()) : MaxColumns;
    
    for (uint32_t i = 0; i < _state._columnCount; ++i) {
        _state._columns[i]._name = columnNames_[i];
        _state._columns[i]._searchLen = 0;
        _state._columns[i]._uniqueCount = 0;
        _state._columns[i]._filterInitialized = false;
        _state._columns[i]._width = 120.0f; // Default width
    }
}

auto TableView::DrawHeader(Rectangle bounds_) -> void 
{
    float currentX = bounds_.x;
    float currentY = bounds_.y;
    const auto& theme = theme::ThemeManager::Get();
    const auto& font = utils::FontManager::GetFont();

    // Draw header background
    DrawRectangle(static_cast<int32_t>(bounds_.x), static_cast<int32_t>(bounds_.y), 
                  static_cast<int32_t>(bounds_.width), static_cast<int32_t>(_state._headerHeight), 
                  theme._headerBg);

    for (uint32_t i = 0; i < _state._columnCount; ++i) {
        // Draw column separator
        DrawLine(static_cast<int32_t>(currentX), static_cast<int32_t>(currentY), 
                 static_cast<int32_t>(currentX), static_cast<int32_t>(currentY + _state._headerHeight), 
                 theme._border);

        // Name
        thread_local std::array<char, MaxCellBufferSize + 1> textBuffer{};
        uint32_t copyLen = static_cast<uint32_t>(_state._columns[i]._name.size());
        if (copyLen > MaxCellBufferSize) [[unlikely]] { copyLen = MaxCellBufferSize; }
        std::memcpy(textBuffer.data(), _state._columns[i]._name.data(), copyLen);
        textBuffer[copyLen] = '\0';

        Vector2 nameSize = MeasureTextEx(font, textBuffer.data(), static_cast<float>(CellFontSize), 0.0f);
        Vector2 namePos = { currentX + 5.0f, currentY + (_state._headerHeight - nameSize.y) / 2.0f };
        DrawTextEx(font, textBuffer.data(), namePos, static_cast<float>(CellFontSize), 0.0f, theme._textPrimary);
        
        // Filter Dropdown Button Icon (Centered in the button box)
        Rectangle btnBox = { currentX + _state._columns[i]._width - 25.0f, currentY + (_state._headerHeight - 20.0f) / 2.0f, 20.0f, 20.0f };
        Color btnColor = (_state._activeDropdownColumn == static_cast<int32_t>(i)) ? theme._surfaceHighlight : theme._surface;
        DrawRectangleRec(btnBox, btnColor);
        DrawRectangleLinesEx(btnBox, 1.0f, theme._border);
        
        Vector2 iconSize = MeasureTextEx(font, ICON_MD_EXPAND_MORE, 16.0f, 0.0f);
        Vector2 iconPos = { 
            btnBox.x + (btnBox.width - iconSize.x) / 2.0f, 
            btnBox.y + (btnBox.height - iconSize.y) / 2.0f 
        };
        DrawTextEx(font, ICON_MD_EXPAND_MORE, iconPos, 16.0f, 0.0f, theme._textPrimary);

        currentX += _state._columns[i]._width;
    }
}

auto TableView::HandleInput(Rectangle bounds_) -> void 
{
    Vector2 mousePos = GetMousePosition();

    // Handle Dropdown Clicks (Priority)
    if (_state._activeDropdownColumn >= 0) {
        auto& activeCol = _state._columns[_state._activeDropdownColumn];
        
        float currentX = bounds_.x;
        for (int32_t i = 0; i < _state._activeDropdownColumn; ++i) {
            currentX += _state._columns[i]._width;
        }

        float dropHeight = 30.0f + (activeCol._uniqueCount * 25.0f);
        Rectangle dropRec = { currentX, bounds_.y + _state._headerHeight, activeCol._width, dropHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, dropRec)) {
                // Clicked inside dropdown. Did we click a checkbox?
                float itemY = dropRec.y + 30.0f;
                for (uint32_t u = 0; u < activeCol._uniqueCount; ++u) {
                    if (CheckCollisionPointRec(mousePos, { dropRec.x, itemY, dropRec.width, 25.0f })) {
                        activeCol._uniqueValues[u]._checked = !activeCol._uniqueValues[u]._checked;
                    }
                    itemY += 25.0f;
                }
            } else {
                // Clicked outside, close dropdown
                _state._activeDropdownColumn = -1;
            }
        }

        // Handle typing for dropdown search
        if (_state._activeDropdownColumn >= 0) {
            if (IsKeyPressed(KEY_BACKSPACE) && activeCol._searchLen > 0) {
                activeCol._searchLen--;
            }
            int32_t key = static_cast<int32_t>(GetCharPressed());
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (activeCol._searchLen < MaxFilterLength)) {
                    activeCol._searchBuffer[activeCol._searchLen] = static_cast<char>(key);
                    activeCol._searchLen++;
                }
                key = static_cast<int32_t>(GetCharPressed());
            }
        }
        return; // Don't process other clicks if dropdown is active
    }

    // Handle Header Clicks to open dropdown
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float currentX = bounds_.x;
        for (uint32_t i = 0; i < _state._columnCount; ++i) {
            Rectangle btnBox = { currentX + _state._columns[i]._width - 25.0f, bounds_.y + (_state._headerHeight - 20.0f) / 2.0f, 20.0f, 20.0f };
            if (CheckCollisionPointRec(mousePos, btnBox)) {
                if (_state._activeDropdownColumn == static_cast<int32_t>(i)) {
                    _state._activeDropdownColumn = -1;
                } else {
                    _state._activeDropdownColumn = static_cast<int32_t>(i);
                }
                break;
            }
            currentX += _state._columns[i]._width;
        }
    }
}

auto TableView::DrawDropdown(Rectangle bounds_) -> void
{
    if (_state._activeDropdownColumn < 0) return;

    auto& activeCol = _state._columns[_state._activeDropdownColumn];
    const auto& theme = theme::ThemeManager::Get();
    const auto& font = utils::FontManager::GetFont();
    
    float currentX = bounds_.x;
    for (int32_t i = 0; i < _state._activeDropdownColumn; ++i) {
        currentX += _state._columns[i]._width;
    }

    float dropHeight = 30.0f + (activeCol._uniqueCount * 25.0f);
    Rectangle dropRec = { currentX, bounds_.y + _state._headerHeight, activeCol._width, dropHeight };

    // Background
    DrawRectangleRec(dropRec, theme._surface);
    DrawRectangleLinesEx(dropRec, 1.0f, theme._border);

    // Search Box
    Rectangle searchBox = { dropRec.x + 5.0f, dropRec.y + 5.0f, dropRec.width - 10.0f, 20.0f };
    DrawRectangleRec(searchBox, theme._background);
    DrawRectangleLinesEx(searchBox, 1.0f, theme._border);
    
    Vector2 searchIconSize = MeasureTextEx(font, ICON_MD_SEARCH, 16.0f, 0.0f);
    Vector2 searchIconPos = { searchBox.x + 4.0f, searchBox.y + (searchBox.height - searchIconSize.y) / 2.0f };
    DrawTextEx(font, ICON_MD_SEARCH, searchIconPos, 16.0f, 0.0f, theme._textSecondary);

    constexpr float TextFontSize = 16.0f;
    thread_local std::array<char, MaxCellBufferSize + 1> textBuffer{};
    if (activeCol._searchLen > 0) {
        std::memcpy(textBuffer.data(), activeCol._searchBuffer.data(), activeCol._searchLen);
        textBuffer[activeCol._searchLen] = '\0';
        
        Vector2 searchTextSize = MeasureTextEx(font, textBuffer.data(), TextFontSize, 0.0f);
        Vector2 searchTextPos = { searchBox.x + 22.0f, searchBox.y + (searchBox.height - searchTextSize.y) / 2.0f };
        DrawTextEx(font, textBuffer.data(), searchTextPos, TextFontSize, 0.0f, theme._textPrimary);
    }

    // Checkboxes
    float itemY = dropRec.y + 30.0f;
    std::string_view searchText(activeCol._searchBuffer.data(), activeCol._searchLen);

    for (uint32_t u = 0; u < activeCol._uniqueCount; ++u) {
        std::string_view itemText(activeCol._uniqueValues[u]._text.data(), activeCol._uniqueValues[u]._len);
        
        // Hide if it doesn't match search
        if (activeCol._searchLen > 0 && itemText.find(searchText) == std::string_view::npos) {
            continue; // We don't shrink the dropdown height dynamically to save complexity, just hide the drawing
        }

        constexpr float IconSize = 18.0f;
        const char* checkIcon = activeCol._uniqueValues[u]._checked ? ICON_MD_CHECK_BOX : ICON_MD_CHECK_BOX_OUTLINE_BLANK;
        Vector2 checkIconSize = MeasureTextEx(font, checkIcon, IconSize, 0.0f);
        Vector2 checkIconPos = { dropRec.x + 5.0f, itemY + (25.0f - checkIconSize.y) / 2.0f };
        
        Color checkColor = activeCol._uniqueValues[u]._checked ? theme._accent : theme._textSecondary;
        DrawTextEx(font, checkIcon, checkIconPos, IconSize, 0.0f, checkColor);

        std::memcpy(textBuffer.data(), itemText.data(), activeCol._uniqueValues[u]._len);
        textBuffer[activeCol._uniqueValues[u]._len] = '\0';
        
        Vector2 labelSize = MeasureTextEx(font, textBuffer.data(), TextFontSize, 0.0f);
        Vector2 labelPos = { dropRec.x + 28.0f, itemY + (25.0f - labelSize.y) / 2.0f };
        DrawTextEx(font, textBuffer.data(), labelPos, TextFontSize, 0.0f, theme._textPrimary);
        
        itemY += 25.0f;
    }
}

} // namespace enma::gui::components
