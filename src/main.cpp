#include "Dashboard.hpp"
#include "Theme.hpp"
#include "FontManager.hpp"
#include "audio/Sound.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

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

static void GlfwErrorCallback(int error_, const char* description_)
{
    spdlog::error("GLFW Error {}: {}", error_, description_);
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

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Enma Trading Engine", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    Theme::SetupGui();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    FontManager::AddIconFonts("resources/fonts/JetBrainsMono.ttf", 16.0f);

    Dashboard dashboard;

    auto UpdateDrawFrame = [&]() -> void {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int32_t display_w = 0;
        int32_t display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        
        // Draw the dashboard UI
        dashboard.Draw(window, display_w, display_h);

        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    };

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg([](void* arg_) {
        auto* loopFunc = static_cast<decltype(&UpdateDrawFrame)>(arg_);
        (*loopFunc)();
    }, &UpdateDrawFrame, 0, 1);
#else
    while (!glfwWindowShouldClose(window)) {
        UpdateDrawFrame();
    }
#endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    AudioEngine::Shutdown();

    return 0;
}
