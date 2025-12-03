#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "redis/protocol.hpp"
#include <string>
#include <vector>

using namespace redis;
using Catch::Matchers::Equals;

TEST_CASE("Protocol: Serialize Simple String", "[protocol]") {
    std::string result = Protocol::serializeSimpleString("OK");
    REQUIRE(result == "+OK\r\n");
    
    result = Protocol::serializeSimpleString("PONG");
    REQUIRE(result == "+PONG\r\n");
}

TEST_CASE("Protocol: Serialize Error", "[protocol]") {
    std::string result = Protocol::serializeError("ERR unknown command");
    REQUIRE(result == "-ERR unknown command\r\n");
    
    result = Protocol::serializeError("WRONGTYPE Operation against a key holding the wrong kind of value");
    REQUIRE(result == "-WRONGTYPE Operation against a key holding the wrong kind of value\r\n");
}

TEST_CASE("Protocol: Serialize Integer", "[protocol]") {
    std::string result = Protocol::serializeInteger(0);
    REQUIRE(result == ":0\r\n");
    
    result = Protocol::serializeInteger(42);
    REQUIRE(result == ":42\r\n");
    
    result = Protocol::serializeInteger(-1);
    REQUIRE(result == ":-1\r\n");
    
    result = Protocol::serializeInteger(123456789);
    REQUIRE(result == ":123456789\r\n");
}

TEST_CASE("Protocol: Serialize Bulk String", "[protocol]") {
    std::string result = Protocol::serializeBulkString("hello");
    REQUIRE(result == "$5\r\nhello\r\n");
    
    result = Protocol::serializeBulkString("");
    REQUIRE(result == "$0\r\n\r\n");
    
    result = Protocol::serializeBulkString("foo bar");
    REQUIRE(result == "$7\r\nfoo bar\r\n");
}

TEST_CASE("Protocol: Serialize Null Bulk String", "[protocol]") {
    std::string result = Protocol::serializeNullBulkString();
    REQUIRE(result == "$-1\r\n");
}

TEST_CASE("Protocol: Serialize Array", "[protocol]") {
    std::vector<std::string> elements = {"foo", "bar"};
    std::string result = Protocol::serializeArray(elements);
    REQUIRE(result == "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n");
    
    elements = {"hello"};
    result = Protocol::serializeArray(elements);
    REQUIRE(result == "*1\r\n$5\r\nhello\r\n");
    
    elements = {};
    result = Protocol::serializeArray(elements);
    REQUIRE(result == "*0\r\n");
}

TEST_CASE("Protocol: Serialize Null Array", "[protocol]") {
    std::string result = Protocol::serializeNullArray();
    REQUIRE(result == "*-1\r\n");
}

TEST_CASE("Protocol: Parse Command - Simple GET", "[protocol]") {
    std::string command = "*2\r\n$3\r\nGET\r\n$3\r\nkey\r\n";
    auto result = Protocol::parseCommand(command);
    
    REQUIRE(result.has_value());
    const auto& args = result.value();
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "GET");
    REQUIRE(args[1] == "key");
}

TEST_CASE("Protocol: Parse Command - SET", "[protocol]") {
    std::string command = "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n";
    auto result = Protocol::parseCommand(command);
    
    REQUIRE(result.has_value());
    const auto& args = result.value();
    REQUIRE(args.size() == 3);
    REQUIRE(args[0] == "SET");
    REQUIRE(args[1] == "key");
    REQUIRE(args[2] == "value");
}

TEST_CASE("Protocol: Parse Command - Empty Array", "[protocol]") {
    std::string command = "*0\r\n";
    auto result = Protocol::parseCommand(command);
    
    REQUIRE(result.has_value());
    const auto& args = result.value();
    REQUIRE(args.size() == 0);
}

TEST_CASE("Protocol: Parse Command - Single Element", "[protocol]") {
    std::string command = "*1\r\n$4\r\nPING\r\n";
    auto result = Protocol::parseCommand(command);
    
    REQUIRE(result.has_value());
    const auto& args = result.value();
    REQUIRE(args.size() == 1);
    REQUIRE(args[0] == "PING");
}

TEST_CASE("Protocol: Parse Command - Multiple Elements", "[protocol]") {
    std::string command = "*5\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n$2\r\nEX\r\n$2\r\n10\r\n";
    auto result = Protocol::parseCommand(command);
    
    REQUIRE(result.has_value());
    const auto& args = result.value();
    REQUIRE(args.size() == 5);
    REQUIRE(args[0] == "SET");
    REQUIRE(args[1] == "key");
    REQUIRE(args[2] == "value");
    REQUIRE(args[3] == "EX");
    REQUIRE(args[4] == "10");
}

TEST_CASE("Protocol: Parse Command - Empty Strings", "[protocol]") {
    std::string command = "*2\r\n$0\r\n\r\n$0\r\n\r\n";
    auto result = Protocol::parseCommand(command);
    
    REQUIRE(result.has_value());
    const auto& args = result.value();
    REQUIRE(args.size() == 2);
    REQUIRE(args[0] == "");
    REQUIRE(args[1] == "");
}

TEST_CASE("Protocol: Parse Command - Returns Error on Invalid Format", "[protocol]") {
    // Missing array indicator
    auto result1 = Protocol::parseCommand("GET\r\n");
    REQUIRE_FALSE(result1.has_value());
    REQUIRE(result1.error().find("must start with '*'") != std::string::npos);
    
    // Missing array length terminator
    auto result2 = Protocol::parseCommand("*2");
    REQUIRE_FALSE(result2.has_value());
    REQUIRE(result2.error().find("missing array length terminator") != std::string::npos);
    
    // Invalid bulk string
    auto result3 = Protocol::parseCommand("*1\r\nGET\r\n");
    REQUIRE_FALSE(result3.has_value());
    REQUIRE(result3.error().find("expected bulk string") != std::string::npos);
    
    // Missing bulk string content
    auto result4 = Protocol::parseCommand("*1\r\n$3\r\n");
    REQUIRE_FALSE(result4.has_value());
    REQUIRE(result4.error().find("bulk string content too short") != std::string::npos);
}

TEST_CASE("Protocol: Round-trip Serialization", "[protocol]") {
    // Test that we can parse what we serialize
    std::vector<std::string> original = {"GET", "mykey"};
    std::string serialized = Protocol::serializeArray(original);
    
    // The serialized array should be parseable
    auto result = Protocol::parseCommand(serialized);
    REQUIRE(result.has_value());
    const auto& parsed = result.value();
    
    REQUIRE(parsed.size() == original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        REQUIRE(parsed[i] == original[i]);
    }
}

TEST_CASE("Protocol: Generic Serialize", "[protocol]") {
    std::string result = Protocol::serialize("OK", ResponseType::SIMPLE_STRING);
    REQUIRE(result == "+OK\r\n");
    
    result = Protocol::serialize("ERR test", ResponseType::ERROR);
    REQUIRE(result == "-ERR test\r\n");
    
    result = Protocol::serialize("42", ResponseType::INTEGER);
    REQUIRE(result == ":42\r\n");
    
    result = Protocol::serialize("hello", ResponseType::BULK_STRING);
    REQUIRE(result == "$5\r\nhello\r\n");
    
    result = Protocol::serialize("", ResponseType::NULL_BULK_STRING);
    REQUIRE(result == "$-1\r\n");
    
    result = Protocol::serialize("", ResponseType::NULL_ARRAY);
    REQUIRE(result == "*-1\r\n");
}

