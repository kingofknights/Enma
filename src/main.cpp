#include "gui/components/TableView.hpp"
#include "gui/utils/Theme.hpp"
#include "gui/utils/FontManager.hpp"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <array>
#include <string_view>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// ==========================================
// MOCK DATA PROVIDER FOR DEMO
// ==========================================

struct TradeRecord {
    std::string_view _symbol;
    std::string_view _side;
    std::string_view _price;
    std::string_view _quantity;
};

class MockTradeProvider {
public:
    static constexpr uint32_t RowCount = 5;

    MockTradeProvider() {
        _data[0] = {"BTC/USD", "BUY", "64302.50", "0.5"};
        _data[1] = {"ETH/USD", "SELL", "3412.10", "12.0"};
        _data[2] = {"SOL/USD", "BUY", "145.20", "100.0"};
        _data[3] = {"BTC/USD", "SELL", "64305.00", "0.1"};
        _data[4] = {"XRP/USD", "BUY", "0.58", "5000.0"};
    }

    [[nodiscard]] auto operator()(uint32_t row_, uint32_t col_) const -> std::string_view {
        if (row_ >= RowCount) [[unlikely]] return "";
        switch (col_) {
            case 0: return _data[row_]._symbol;
            case 1: return _data[row_]._side;
            case 2: return _data[row_]._price;
            case 3: return _data[row_]._quantity;
            default: return "";
        }
    }

private:
    std::array<TradeRecord, RowCount> _data;
};

// ==========================================
// APPLICATION STATE
// ==========================================

constexpr int32_t screenWidth = 800;
constexpr int32_t screenHeight = 450;

// Global components for the demo
MockTradeProvider g_tradeProvider;
std::array<std::string_view, 4> g_columns = {"Symbol", "Side", "Price", "Qty"};
enma::gui::components::TableView g_tableView(g_columns);

// ==========================================
// MAIN LOOP
// ==========================================

void UpdateDrawFrame(void)
{
    BeginDrawing();
    ClearBackground(enma::gui::theme::ThemeManager::Get()._background);
    
    // Define the bounding box for the table
    Rectangle tableBounds = { 20.0f, 20.0f, static_cast<float>(screenWidth - 40), static_cast<float>(screenHeight - 40) };
    
    // Handle Input & Draw Table (Using the unified zero-allocation UpdateAndDraw method)
    g_tableView.UpdateAndDraw(tableBounds, MockTradeProvider::RowCount, g_tradeProvider);

    EndDrawing();
}

auto main() -> int 
{
    // Initialize Async Logger
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/rain.log", true);
    
    std::vector<spdlog::sink_ptr> sinks {stdout_sink, file_sink};
    auto async_logger = std::make_shared<spdlog::async_logger>("async_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    
    spdlog::set_default_logger(async_logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
    spdlog::info("Enma Trading Engine starting... 12 cores active, Production environment");

    // Initialize Raylib
    InitWindow(screenWidth, screenHeight, "Enma Trading Engine");
    
    // Initialize UI Assets
    enma::gui::utils::FontManager::Initialize();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    // Cleanup
    enma::gui::utils::FontManager::Unload();
    CloseWindow();

    return 0;
}
