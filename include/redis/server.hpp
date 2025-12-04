#pragma once

#include "database.hpp"
#include "client_connection.hpp"
#include "protocol.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>

namespace redis {

// Redis server
class Server {
public:
    Server(const std::string& host = "127.0.0.1", int port = 6379);
    ~Server();
    
    // Start the server
    void start();
    
    // Stop the server
    void stop();
    
    // Check if server is running
    bool isRunning() const;
    
    // Get server address and port
    std::string getHost() const;
    int getPort() const;
    
private:
    std::string host_;
    int port_;
    int server_socket_;
    int epoll_fd_;
    std::atomic<bool> running_;
    std::unordered_map<int, std::unique_ptr<ClientConnection>> connections_;
    Database database_;
    
    void acceptConnections();
    void handleClient(int client_socket);
};

} // namespace redis

