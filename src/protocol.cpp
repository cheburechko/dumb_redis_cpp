#include "redis/protocol.hpp"
#include <sstream>
#include <algorithm>

namespace redis {

std::expected<CommandArgs, std::string> Protocol::parseCommand(const std::string& data) {
    CommandArgs args;
    
    if (data.empty() || data[0] != '*') {
        return std::unexpected("Invalid RESP command: must start with '*'");
    }
    
    size_t pos = 1;
    
    // Parse array length
    size_t array_end = data.find("\r\n", pos);
    if (array_end == std::string::npos) {
        return std::unexpected("Invalid RESP command: missing array length terminator");
    }
    
    int array_length;
    try {
        array_length = std::stoi(data.substr(pos, array_end - pos));
    } catch (const std::exception&) {
        return std::unexpected("Invalid RESP command: invalid array length");
    }
    pos = array_end + 2;
    
    // Parse each bulk string in the array
    for (int i = 0; i < array_length; ++i) {
        if (pos >= data.length() || data[pos] != '$') {
            return std::unexpected("Invalid RESP command: expected bulk string");
        }
        
        pos++; // Skip '$'
        
        // Parse bulk string length
        size_t length_end = data.find("\r\n", pos);
        if (length_end == std::string::npos) {
            return std::unexpected("Invalid RESP command: missing bulk string length terminator");
        }
        
        int bulk_length;
        try {
            bulk_length = std::stoi(data.substr(pos, length_end - pos));
        } catch (const std::exception&) {
            return std::unexpected("Invalid RESP command: invalid bulk string length");
        }
        pos = length_end + 2;
        
        // Extract bulk string content
        if (pos + bulk_length + 2 > data.length()) {
            return std::unexpected("Invalid RESP command: bulk string content too short");
        }
        
        std::string arg = data.substr(pos, bulk_length);
        args.push_back(arg);
        pos += bulk_length + 2; // Skip content and \r\n
    }
    
    return args;
}

std::string Protocol::serializeSimpleString(const std::string& str) {
    return "+" + str + "\r\n";
}

std::string Protocol::serializeError(const std::string& error) {
    return "-" + error + "\r\n";
}

std::string Protocol::serializeInteger(int64_t value) {
    return ":" + std::to_string(value) + "\r\n";
}

std::string Protocol::serializeBulkString(const std::string& str) {
    return "$" + std::to_string(str.length()) + "\r\n" + str + "\r\n";
}

std::string Protocol::serializeNullBulkString() {
    return "$-1\r\n";
}

std::string Protocol::serializeArray(const std::vector<std::string>& elements) {
    std::string result = "*" + std::to_string(elements.size()) + "\r\n";
    for (const auto& elem : elements) {
        result += serializeBulkString(elem);
    }
    return result;
}

std::string Protocol::serializeNullArray() {
    return "*-1\r\n";
}

std::string Protocol::serialize(const std::string& response, ResponseType type) {
    switch (type) {
        case ResponseType::SIMPLE_STRING:
            return serializeSimpleString(response);
        case ResponseType::ERROR:
            return serializeError(response);
        case ResponseType::INTEGER:
            return serializeInteger(std::stoll(response));
        case ResponseType::BULK_STRING:
            return serializeBulkString(response);
        case ResponseType::NULL_BULK_STRING:
            return serializeNullBulkString();
        case ResponseType::ARRAY:
            // For arrays, response should be comma-separated or we need a different approach
            // This is a simplified version
            return serializeBulkString(response);
        case ResponseType::NULL_ARRAY:
            return serializeNullArray();
        default:
            return serializeError("Unknown response type");
    }
}

} // namespace redis

