#include "Dashboard.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "IconsMaterialDesign.h"
#include "spdlog/spdlog.h"
#include <GLFW/glfw3.h>
#include <cfloat>

Dashboard::Dashboard() : _sideOptions{"BUY", "SELL"}
{
    _tradeData[0] = {"BTC/USD", "BUY", "64302.50", "0.5"};
    _tradeData[1] = {"ETH/USD", "SELL", "3412.10", "12.0"};
    _tradeData[2] = {"SOL/USD", "BUY", "145.20", "100.0"};
    _tradeData[3] = {"BTC/USD", "SELL", "64305.00", "0.1"};
    _tradeData[4] = {"XRP/USD", "BUY", "0.58", "5000.0"};
}

auto Dashboard::DrawHeader(GLFWwindow* window_, float height_) -> void
{
    ImGuiWindowFlags menuBarFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

    if (ImGui::BeginViewportSideBar("TopSecondMenu##SecondaryMenuBar", nullptr, ImGuiDir_Up, height_, menuBarFlags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::Text(ICON_MD_WATER_DROP " ENMA TRADING ENGINE");

            const char* statusText = ICON_MD_CHECK_CIRCLE " SYSTEM: ONLINE";
            float windowControlsWidth = 120.0f;
            ImGui::SameLine(ImGui::GetWindowWidth() - windowControlsWidth - ImGui::CalcTextSize(statusText).x - 20.0f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", statusText);

            ImGui::SameLine(ImGui::GetWindowWidth() - windowControlsWidth);
            if (ImGui::MenuItem(ICON_MD_REMOVE)) {
                glfwIconifyWindow(window_);
            }

            bool isMaximized = glfwGetWindowAttrib(window_, GLFW_MAXIMIZED);
            const char* maxIcon = isMaximized ? ICON_MD_FILTER_NONE : ICON_MD_CROP_SQUARE;
            if (ImGui::MenuItem(maxIcon)) {
                if (isMaximized) {
                    glfwRestoreWindow(window_);
                } else {
                    glfwMaximizeWindow(window_);
                }
            }

            if (ImGui::MenuItem(ICON_MD_CLOSE)) {
                glfwSetWindowShouldClose(window_, GLFW_TRUE);
            }

            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
}

auto Dashboard::DrawFooter(float height_) -> void
{
    ImGuiWindowFlags menuBarFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

    if (ImGui::BeginViewportSideBar("StatusBar##MainStatusBar", nullptr, ImGuiDir_Down, height_, menuBarFlags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ICON_MD_WIFI " Network: 1ms");
            ImGui::SameLine(150.0f);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ICON_MD_MEMORY " Memory: 14 MB");

            const char* clockText = ICON_MD_SCHEDULE " 14:02:30 UTC";
            ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(clockText).x - 15.0f);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", clockText);

            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
}

auto Dashboard::Draw(GLFWwindow* window_, int32_t displayWidth_, int32_t displayHeight_) -> void
{
    float height = ImGui::GetFrameHeight();

    DrawHeader(window_, height);
    DrawFooter(height);

    // Main Content Window
    ImGui::SetNextWindowPos(ImVec2(0.0f, height));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(displayWidth_), static_cast<float>(displayHeight_) - (height * 2.0f)));

    if (ImGui::Begin("MainWorkspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus)) {
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
                
                int currentSideIdx = (_tradeData[row]._side == "BUY") ? 0 : 1;
                
                ImGui::SetNextItemWidth(-FLT_MIN);
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
    ImGui::End();
}
