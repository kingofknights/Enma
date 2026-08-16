#include "TcpConnection.hpp"

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#elif defined(__linux__)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

namespace enma::core::network {

TcpConnection::TcpConnection(std::string_view ipAddress_, uint16_t port_) 
{
    _state._ipAddress = std::string(ipAddress_);
    _state._port = port_;
    _state._socketFd = -1;

    for (uint32_t i = 0; i < 6; ++i) {
        _state._padding[i] = 0;
    }
}

TcpConnection::~TcpConnection() 
{
    Close();
}

auto TcpConnection::Initialize() -> bool 
{
#if defined(__EMSCRIPTEN__)
    return false;
#else
#if defined(_WIN32)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }
#endif

    _state._socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_state._socketFd < 0) {
        return false;
    }

    return true;
#endif
}

auto TcpConnection::Connect() -> bool 
{
#if defined(__EMSCRIPTEN__)
    return false;
#else
    if (_state._socketFd < 0) {
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_state._port);
    
#if defined(_WIN32)
    serverAddr.sin_addr.s_addr = inet_addr(_state._ipAddress.c_str());
#elif defined(__linux__)
    inet_pton(AF_INET, _state._ipAddress.c_str(), &serverAddr.sin_addr);
#endif

    // Connect (blocking mode is fine during startup phase)
    if (connect(static_cast<int32_t>(_state._socketFd), reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        Close();
        return false;
    }

    // Now switch socket to non-blocking for ultra-low latency Receive() on the hot path
#if defined(_WIN32)
    u_long mode = 1;
    ioctlsocket(static_cast<SOCKET>(_state._socketFd), FIONBIO, &mode);
    
    // Disable Nagle's algorithm
    const char noDelay = 1;
    setsockopt(static_cast<SOCKET>(_state._socketFd), IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay));
#elif defined(__linux__)
    int32_t flags = fcntl(static_cast<int32_t>(_state._socketFd), F_GETFL, 0);
    fcntl(static_cast<int32_t>(_state._socketFd), F_SETFL, flags | O_NONBLOCK);
    
    // Disable Nagle's algorithm
    int32_t noDelay = 1;
    setsockopt(static_cast<int32_t>(_state._socketFd), IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay));
#endif

    return true;
#endif
}

auto TcpConnection::Receive(std::span<uint8_t> buffer_) -> int32_t 
{
#if defined(__EMSCRIPTEN__)
    return -1;
#else
    if (_state._socketFd < 0) {
        return -1;
    }

#if defined(_WIN32)
    int32_t bytesReceived = recv(static_cast<SOCKET>(_state._socketFd), reinterpret_cast<char*>(buffer_.data()), static_cast<int>(buffer_.size()), 0);
#elif defined(__linux__)
    int32_t bytesReceived = static_cast<int32_t>(recv(static_cast<int32_t>(_state._socketFd), buffer_.data(), buffer_.size(), 0));
#endif

    return bytesReceived;
#endif
}

auto TcpConnection::Close() -> void 
{
#if !defined(__EMSCRIPTEN__)
    if (_state._socketFd >= 0) {
#if defined(_WIN32)
        closesocket(static_cast<SOCKET>(_state._socketFd));
        WSACleanup();
#elif defined(__linux__)
        close(static_cast<int32_t>(_state._socketFd));
#endif
        _state._socketFd = -1;
    }
#endif
}

} // namespace enma::core::network
