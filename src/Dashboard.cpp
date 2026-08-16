#include "Dashboard.hpp"
#include "imgui.h"
#include "IconsMaterialDesign.h"
#include "spdlog/spdlog.h"
#include <cfloat>

Dashboard::Dashboard() : _sideOptions{"BUY", "SELL"}
{
    _tradeData[0] = {"BTC/USD", "BUY", "64302.50", "0.5"};
    _tradeData[1] = {"ETH/USD", "SELL", "3412.10", "12.0"};
    _tradeData[2] = {"SOL/USD", "BUY", "145.20", "100.0"};
    _tradeData[3] = {"BTC/USD", "SELL", "64305.00", "0.1"};
    _tradeData[4] = {"XRP/USD", "BUY", "0.58", "5000.0"};
}

#include <GLFW/glfw3.h> // Include GLFW for window control functions

auto Dashboard::Draw(GLFWwindow* window_, int32_t displayWidth_, int32_t displayHeight_) -> void
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(displayWidth_), static_cast<float>(displayHeight_)));
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (ImGui::Begin("MainWorkspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus)) {
        
        float headerHeight = 40.0f;
        float footerHeight = 30.0f;
        float mainHeight = static_cast<float>(displayHeight_) - headerHeight - footerHeight;

        // 1. Header Bar
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.15f, 0.18f, 1.00f));
        if (ImGui::BeginChild("HeaderBar", ImVec2(0, headerHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            ImGui::SetCursorPosY((headerHeight - ImGui::GetTextLineHeight()) * 0.5f);
            ImGui::SetCursorPosX(15.0f);
            ImGui::Text(ICON_MD_WATER_DROP " ENMA TRADING ENGINE");
            
            // Layout dimensions for right-side items
            float btnWidth = 45.0f;
            float btnHeight = headerHeight;
            float totalButtonsWidth = btnWidth * 3.0f;
            
            const char* statusText = ICON_MD_CHECK_CIRCLE " SYSTEM: ONLINE";
            float textWidth = ImGui::CalcTextSize(statusText).x;

            // Draw Status Text (shifted left to make room for buttons)
            ImGui::SameLine(ImGui::GetWindowWidth() - totalButtonsWidth - textWidth - 25.0f);
            ImGui::SetCursorPosY((headerHeight - ImGui::GetTextLineHeight()) * 0.5f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", statusText);

            ImGui::SameLine(ImGui::GetWindowWidth() - totalButtonsWidth);
            ImGui::SetCursorPosY(0.0f);
            
            // Helper lambda for perfectly centered icon buttons (bypassing ImGui::Button's rigid alignment)
            auto DrawWindowButton = [&](const char* id, const char* icon, float width, float height, float iconYOffset, bool isClose) -> bool {
                ImVec2 pos = ImGui::GetCursorScreenPos();
                bool clicked = ImGui::InvisibleButton(id, ImVec2(width, height));
                bool hovered = ImGui::IsItemHovered();
                bool active = ImGui::IsItemActive();

                ImU32 bgCol = IM_COL32(0, 0, 0, 0);
                if (hovered) {
                    if (isClose) {
                        bgCol = active ? IM_COL32(230, 50, 50, 255) : IM_COL32(200, 50, 50, 255);
                    } else {
                        bgCol = active ? IM_COL32(255, 255, 255, 50) : IM_COL32(255, 255, 255, 25);
                    }
                }
                
                ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bgCol);

                ImVec2 textSize = ImGui::CalcTextSize(icon);
                ImVec2 textPos(pos.x + (width - textSize.x) * 0.5f, pos.y + (height - textSize.y) * 0.5f + iconYOffset);
                ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), icon);

                return clicked;
            };

            // Fine-tuned Y offsets to counteract Material Design baseline weirdness
            if (DrawWindowButton("##Min", ICON_MD_REMOVE, btnWidth, btnHeight, 1.0f, false)) {
                glfwIconifyWindow(window_);
            }

            ImGui::SameLine(0, 0);
            bool isMaximized = glfwGetWindowAttrib(window_, GLFW_MAXIMIZED);
            const char* maxIcon = isMaximized ? ICON_MD_FILTER_NONE : ICON_MD_CROP_SQUARE;
            // Crop Square needs slight upward shift, Filter None needs slightly more upward shift
            float maxYOffset = isMaximized ? -3.0f : -2.0f; 
            if (DrawWindowButton("##Max", maxIcon, btnWidth, btnHeight, maxYOffset, false)) {
                if (isMaximized) {
                    glfwRestoreWindow(window_);
                } else {
                    glfwMaximizeWindow(window_);
                }
            }

            ImGui::SameLine(0, 0);
            if (DrawWindowButton("##Close", ICON_MD_CLOSE, btnWidth, btnHeight, -1.0f, true)) {
                glfwSetWindowShouldClose(window_, GLFW_TRUE);
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        // 2. Main Content
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        if (ImGui::BeginChild("MainContent", ImVec2(0, mainHeight), false, ImGuiWindowFlags_None)) {
            ImGui::SetCursorPos(ImVec2(10.0f, 10.0f));
            ImGui::Text(ICON_MD_DASHBOARD " Trading Dashboard");
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::BeginTable("TradesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Symbol");
                ImGui::TableSetupColumn("Side");
                ImGui::TableSetupColumn("Price");
                ImGui::TableSetupColumn("Quantity");
                ImGui::TableHeadersRow();

                for (uint32_t row = 0; row < RowCount; row++) {
                    ImGui::TableNextRow();
                    
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(_tradeData[row]._symbol.data());
                    
                    ImGui::TableNextColumn();
                    ImGui::PushID(static_cast<int>(row));
                    
                    // Find current side index
                    int currentSideIdx = (_tradeData[row]._side == "BUY") ? 0 : 1;
                    
                    ImGui::SetNextItemWidth(-FLT_MIN); // Fill cell width
                    if (ImGui::Combo("##SideCombo", &currentSideIdx, _sideOptions.data(), static_cast<int>(_sideOptions.size()))) {
                        _tradeData[row]._side = _sideOptions[static_cast<size_t>(currentSideIdx)];
                        spdlog::info("Row {} side changed to: {}", row, _sideOptions[static_cast<size_t>(currentSideIdx)]);
                    }
                    ImGui::PopID();
                    
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(_tradeData[row]._price.data());
                    
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(_tradeData[row]._quantity.data());
                }
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();

        // 3. Footer Status Bar
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.12f, 1.00f));
        if (ImGui::BeginChild("FooterBar", ImVec2(0, footerHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            float yPos = (footerHeight - ImGui::GetTextLineHeight()) * 0.5f;
            
            ImGui::SetCursorPosY(yPos);
            ImGui::SetCursorPosX(15.0f);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ICON_MD_WIFI " Network: 1ms");
            
            ImGui::SameLine(150.0f);
            ImGui::SetCursorPosY(yPos);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ICON_MD_MEMORY " Memory: 14 MB");
            
            // Clock on right
            const char* clockText = ICON_MD_SCHEDULE " 14:02:30 UTC";
            float clockWidth = ImGui::CalcTextSize(clockText).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - clockWidth - 15.0f);
            ImGui::SetCursorPosY(yPos);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", clockText);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

    }
    ImGui::End();
    ImGui::PopStyleVar();
}
