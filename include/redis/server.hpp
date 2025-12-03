#pragma once

#include "database.hpp"
#include "protocol.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <functional>

namespace redis {

// Client connection handler
class ClientConnection {
public:
    ClientConnection(int socket_fd, Database& db);
    ~ClientConnection();
    
    void handle();
    void close();
    bool isActive() const;
    
private:
    int socket_fd_;
    Database& database_;
    std::atomic<bool> active_;
    
    std::string readRequest();
    void sendResponse(const std::string& response);
};

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
    std::atomic<bool> running_;
    std::vector<std::unique_ptr<ClientConnection>> connections_;
    Database database_;
    
    void acceptConnections();
    void handleClient(int client_socket);
    void setupSocket();
    void bindAndListen();
};

} // namespace redis

