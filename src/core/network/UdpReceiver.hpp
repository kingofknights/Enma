#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <string>

namespace enma::core::network {

class UdpReceiver {
public:
    UdpReceiver(std::string_view ipAddress_, uint16_t port_);
    ~UdpReceiver();

    [[nodiscard]] auto Initialize() -> bool;
    [[nodiscard]] auto Receive(std::span<uint8_t> buffer_) -> int32_t;
    auto Close() -> void;

private:
    struct PrivateData {
        std::string _ipAddress;
        int64_t _socketFd; // 8 bytes (Use int64_t for socket fd to handle 64-bit windows sockets if needed, but Linux uses int)
        uint16_t _port;    // 2 bytes
        uint8_t _padding[6]; // 6 bytes of padding to align to 64-bit boundary for cache friendliness
    };

    PrivateData _state;
};

} // namespace enma::core::network
