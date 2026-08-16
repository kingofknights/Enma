#pragma once

#include <cstdint>
#include <string_view>
#include <array>

struct TradeRecord {
    std::string_view _symbol;
    std::string_view _side;
    std::string_view _price;
    std::string_view _quantity;
};

struct GLFWwindow;

class Dashboard {
public:
    Dashboard();
    ~Dashboard() = default;

    auto Draw(GLFWwindow* window_, int32_t displayWidth_, int32_t displayHeight_) -> void;

private:
    auto DrawHeader(GLFWwindow* window_, float height_) -> void;
    auto DrawFooter(float height_) -> void;

    static constexpr uint32_t RowCount = 5;
    using TradeRecordArrayT = std::array<TradeRecord, RowCount>;
    using SideOptionsT = std::array<const char*, 2>;

    TradeRecordArrayT _tradeData;
    SideOptionsT _sideOptions;
};
