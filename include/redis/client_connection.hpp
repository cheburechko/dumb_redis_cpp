#pragma once

#include <string>
#include <sstream>
#include <atomic>

namespace redis {

// Forward declaration
class Database;

// Client connection handler
class ClientConnection {
public:
    ClientConnection(int socket_fd, Database& db);
    ~ClientConnection();
    
    void handle();
    void close();
    bool isActive() const;
    bool hasPendingData() const;
    
private:
    int socket_fd_;
    std::stringstream buffer_;
    Database& database_;
    std::atomic<bool> active_;
    
    std::string readRequest();
    void sendResponse(const std::string& response);
};

} // namespace redis

