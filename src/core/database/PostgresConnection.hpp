#pragma once

#include <cstdint>
#include <memory>

namespace enma::core::database {

class PostgresConnection {
public:
    // IMPORTANT: PostgreSQL connections allocate memory heavily and do blocking I/O.
    // This class MUST ONLY be used during application initialization, end-of-day reporting,
    // or entirely on an isolated non-critical logging thread. NEVER on the hot path.
    PostgresConnection(const char* connectionString_);
    ~PostgresConnection();

    [[nodiscard]] auto Connect() -> bool;
    auto Disconnect() -> void;

    // Example query execution method.
    // In a real ultra-low latency system, you would pass pre-formatted statements and avoid strings.
    [[nodiscard]] auto Execute(const char* query_) -> bool;

private:
    struct PrivateData; // Pimpl idiom to hide pqxx dependencies from our low-latency headers
    std::unique_ptr<PrivateData> _data;
};

} // namespace enma::core::database
