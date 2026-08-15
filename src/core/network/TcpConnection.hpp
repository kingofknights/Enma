#pragma once

#include <cstdint>

namespace enma::core::network {

class TcpConnection {
public:
    TcpConnection(const char* ipAddress_, uint16_t port_);
    ~TcpConnection();

    [[nodiscard]] auto Initialize() -> bool;
    
    // Connects to the remote server (blocking during handshake, then switches to non-blocking)
    [[nodiscard]] auto Connect() -> bool;
    
    // Receives data from the server into a pre-allocated buffer
    [[nodiscard]] auto Receive(uint8_t* buffer_, uint32_t bufferSize_) -> int32_t;
    
    auto Close() -> void;

private:
    struct PrivateData {
        const char* _ipAddress;
        int64_t _socketFd;
        uint16_t _port;
        uint8_t _padding[6]; // Padding to maintain 64-bit boundary alignment
    };

    PrivateData _state;
};

} // namespace enma::core::network
