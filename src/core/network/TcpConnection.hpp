#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <string>

namespace enma::core::network {

class TcpConnection {
public:
    TcpConnection(std::string_view ipAddress_, uint16_t port_);
    ~TcpConnection();

    [[nodiscard]] auto Initialize() -> bool;
    
    // Connects to the remote server (blocking during handshake, then switches to non-blocking)
    [[nodiscard]] auto Connect() -> bool;
    
    // Receives data from the server into a pre-allocated buffer
    [[nodiscard]] auto Receive(std::span<uint8_t> buffer_) -> int32_t;
    
    auto Close() -> void;

private:
    struct PrivateData {
        std::string _ipAddress;
        int64_t _socketFd;
        uint16_t _port;
        uint8_t _padding[6]; // Padding to maintain 64-bit boundary alignment
    };

    PrivateData _state;
};

} // namespace enma::core::network
