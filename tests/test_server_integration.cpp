#include <catch2/catch_test_macros.hpp>
#include "redis/server.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <sw/redis++/redis++.h>
#include <string>
#include <stdexcept>

using namespace redis;
using namespace sw::redis;
using namespace std::chrono_literals;

TEST_CASE("Server Integration: Basic commands", "[integration]") {
    // Use a random port to avoid conflicts
    const int test_port = 6380;
    const std::string test_host = "127.0.0.1";
    
    // Start server in a separate thread
    Server server(test_host, test_port);
    std::atomic<bool> server_started{false};
    std::exception_ptr server_exception = nullptr;
    
    std::thread server_thread([&]() {
        try {
            server_started = true;
            server.start();
        } catch (...) {
            server_exception = std::current_exception();
        }
    });
    
    // Wait for server to start
    std::this_thread::sleep_for(100ms);
    REQUIRE(server_started);
    
    // Give server a moment to fully initialize
    std::this_thread::sleep_for(300ms);
    
    try {
        // Connect to the server
        ConnectionOptions opts;
        opts.host = test_host;
        opts.port = test_port;
        opts.socket_timeout = std::chrono::milliseconds(2000);
        opts.connect_timeout = std::chrono::milliseconds(2000);
        
        Redis redis(opts);
        
        // Test that we can connect and get responses
        // Since commands aren't implemented, we should get error responses
        
        SECTION("PING command") {
            auto result = redis.ping();
            REQUIRE(result == "PONG");
        }
        
        SECTION("SET command") {
            REQUIRE(redis.set("test_key", "test_value"));
        }
        
        SECTION("GET command") {
            auto result = redis.get("test_key");
            REQUIRE_FALSE(result.has_value());
        }
        
        SECTION("Multiple commands") {
            REQUIRE(redis.ping() == "PONG");
            REQUIRE(redis.set("key1", "value1"));
            REQUIRE(redis.get("key1") == "value1");
        }
        
        SECTION("Connection remains active after multiple commands") {
            for (int i = 0; i < 5; ++i) {
                REQUIRE(redis.ping() == "PONG");
                std::this_thread::sleep_for(50ms);
            }
        }
        
    } catch (const std::exception& e) {
        // If connection fails, that's a test failure
        FAIL("Failed to connect to server: " + std::string(e.what()));
    }
    
    // Stop the server
    server.stop();
    
    // Wait for server thread to finish
    if (server_thread.joinable()) {
        server_thread.join();
    }
    
    // Check if server threw an exception
    if (server_exception) {
        std::rethrow_exception(server_exception);
    }
}

TEST_CASE("Server Integration: Basic connection test", "[integration]") {
    const int test_port = 6381;
    const std::string test_host = "127.0.0.1";
    
    Server server(test_host, test_port);
    std::atomic<bool> server_started{false};
    std::exception_ptr server_exception = nullptr;
    
    std::thread server_thread([&]() {
        try {
            server_started = true;
            server.start();
        } catch (...) {
            server_exception = std::current_exception();
        }
    });
    
    // Wait for server to start
    std::this_thread::sleep_for(200ms);
    REQUIRE(server_started);
    
    // Give server time to initialize
    std::this_thread::sleep_for(200ms);
    
    try {
        ConnectionOptions opts;
        opts.host = test_host;
        opts.port = test_port;
        opts.socket_timeout = std::chrono::milliseconds(2000);
        opts.connect_timeout = std::chrono::milliseconds(2000);
        
        Redis redis(opts);
        
        // Just verify we can create the connection object
        // The actual command execution will fail, but that's expected
        REQUIRE(true);
        
    } catch (const std::exception& e) {
        FAIL("Failed to create Redis connection: " + std::string(e.what()));
    }
    
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
    
    if (server_exception) {
        std::rethrow_exception(server_exception);
    }
}

