#include "Dashboard.hpp"
#include "audio/Sound.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "IconsMaterialDesign.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <memory>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

constexpr int32_t screenWidth = 800;
constexpr int32_t screenHeight = 450;

Dashboard g_dashboard;
GLFWwindow* g_window = nullptr;

static void GlfwErrorCallback(int error_, const char* description_)
{
    spdlog::error("GLFW Error {}: {}", error_, description_);
}

void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    style.FrameRounding = 4.0f;
}

auto AddIconFonts(const std::string& ttf_, float size_) -> void {
    ImGuiIO& io = ImGui::GetIO();
    float baseFontSize = size_;
    float iconFontSize = baseFontSize;

    static constexpr ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode   = true;
    icons_config.PixelSnapH  = true;
    icons_config.GlyphOffset = ImVec2(0, 4);
    
    io.Fonts->AddFontFromFileTTF(ttf_.data(), iconFontSize);
    io.Fonts->AddFontFromFileTTF("resources/fonts/MaterialIcons-Regular.ttf", iconFontSize, &icons_config, icons_ranges);

    spdlog::info("Adding Fonts Style {} : {}", ttf_, iconFontSize);
    spdlog::info("Adding Fonts Icon {} : {}", "resources/fonts/MaterialIcons-Regular.ttf", iconFontSize);
}

void UpdateDrawFrame()
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int display_w, display_h;
    glfwGetFramebufferSize(g_window, &display_w, &display_h);
    
    // Draw the dashboard UI
    g_dashboard.Draw(g_window, display_w, display_h);

    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(g_window);
}

auto main() -> int 
{
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/rain.log", true);
    
    std::vector<spdlog::sink_ptr> sinks {stdout_sink, file_sink};
    auto async_logger = std::make_shared<spdlog::async_logger>("async_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    
    spdlog::set_default_logger(async_logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    spdlog::info("Enma Trading Engine starting... 12 cores active, Production environment");

    if (!AudioEngine::Initialize()) {
        spdlog::warn("AudioEngine failed to initialize. Sound features will be disabled.");
    }

    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit())
        return 1;

#if defined(PLATFORM_WEB)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
    
    // Remove borders/title bar and open maximized
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    g_window = glfwCreateWindow(screenWidth, screenHeight, "Enma Trading Engine", nullptr, nullptr);
    if (g_window == nullptr)
        return 1;
    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    SetupImGuiStyle();

    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    AddIconFonts("resources/fonts/JetBrainsMono.ttf", 16.0f);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!glfwWindowShouldClose(g_window)) {
        UpdateDrawFrame();
    }
#endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(g_window);
    glfwTerminate();

    AudioEngine::Shutdown();

    return 0;
}
