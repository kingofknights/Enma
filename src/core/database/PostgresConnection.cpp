#include "PostgresConnection.hpp"

#if !defined(__EMSCRIPTEN__)
#include <pqxx/pqxx>
#include <iostream>
#endif

namespace enma::core::database {

#if !defined(__EMSCRIPTEN__)
struct PostgresConnection::PrivateData {
    std::unique_ptr<pqxx::connection> _conn;
    std::string _connectionString;
};
#else
struct PostgresConnection::PrivateData {
    // Empty stub for WebAssembly which cannot natively connect to PostgreSQL
};
#endif

PostgresConnection::PostgresConnection(std::string_view connectionString_)
    : _data(std::make_unique<PrivateData>())
{
#if !defined(__EMSCRIPTEN__)
    _data->_connectionString = std::string(connectionString_);
#endif
}

PostgresConnection::~PostgresConnection() 
{
    Disconnect();
}

auto PostgresConnection::Connect() -> bool 
{
#if defined(__EMSCRIPTEN__)
    // PostgreSQL connections are physically blocked in WebAssembly browser sandboxes
    return false; 
#else
    try {
        _data->_conn = std::make_unique<pqxx::connection>(_data->_connectionString);
        return _data->_conn->is_open();
    } catch (const std::exception& e_) {
        // Logging is acceptable here since Connect() is only called during initialization
        std::cerr << "PostgresConnection Error: " << e_.what() << "\n";
        return false;
    }
#endif
}

auto PostgresConnection::Disconnect() -> void 
{
#if !defined(__EMSCRIPTEN__)
    _data->_conn.reset();
#endif
}

auto PostgresConnection::Execute(std::string_view query_) -> bool 
{
#if defined(__EMSCRIPTEN__)
    return false;
#else
    if (!_data->_conn || !_data->_conn->is_open()) {
        return false;
    }

    try {
        // pqxx::work initiates a transaction which is heavily allocating and blocking.
        // Again, this is strictly forbidden in the trading hot-path.
        pqxx::work txn(*_data->_conn);
        txn.exec(std::string(query_));
        txn.commit();
        return true;
    } catch (const std::exception& e_) {
        std::cerr << "PostgresConnection Execute Error: " << e_.what() << "\n";
        return false;
    }
#endif
}

} // namespace enma::core::database
