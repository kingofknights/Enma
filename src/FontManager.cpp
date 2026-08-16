#include "FontManager.hpp"
#include "imgui.h"
#include "IconsMaterialDesign.h"
#include "spdlog/spdlog.h"
#include <string>

auto FontManager::AddIconFonts(std::string_view ttf_, float size_) -> void
{
    ImGuiIO& io = ImGui::GetIO();

    static constexpr ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode   = true;
    icons_config.PixelSnapH  = true;
    icons_config.GlyphOffset = ImVec2(0, 4);
    
    std::string ttfPath{ttf_};
    io.Fonts->AddFontFromFileTTF(ttfPath.c_str(), size_);
    io.Fonts->AddFontFromFileTTF("resources/fonts/MaterialIcons-Regular.ttf", size_, &icons_config, icons_ranges);
   
    spdlog::info("Adding Fonts Style {} : {}", ttf_, size_);
    spdlog::info("Adding Fonts Icon {} : {}", "resources/fonts/MaterialIcons-Regular.ttf", size_);
   
}
