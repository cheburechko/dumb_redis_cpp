#include <iostream>
#include <signal.h>
#include "redis/server.hpp"

static redis::Server* g_server = nullptr;

void signalHandler(int signal) {
    if (g_server != nullptr) {
        std::cout << "\nShutting down server..." << std::endl;
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string host = "127.0.0.1";
    int port = 6379;
    
    // TODO: Add argument parsing
    
    // Create and start server
    redis::Server server(host, port);
    g_server = &server;
    
    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "Starting Redis server on " << host << ":" << port << std::endl;
    
    try {
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

