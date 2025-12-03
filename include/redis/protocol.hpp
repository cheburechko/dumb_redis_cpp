#pragma once

#include "types.hpp"
#include <string>
#include <vector>

namespace redis {

// RESP (Redis Serialization Protocol) handler
class Protocol {
public:
    // Parse incoming RESP command
    static CommandArgs parseCommand(const std::string& data);
    
    // Serialize response to RESP format
    static std::string serializeSimpleString(const std::string& str);
    static std::string serializeError(const std::string& error);
    static std::string serializeInteger(int64_t value);
    static std::string serializeBulkString(const std::string& str);
    static std::string serializeNullBulkString();
    static std::string serializeArray(const std::vector<std::string>& elements);
    static std::string serializeNullArray();
    
    // Generic serialize based on response type
    static std::string serialize(const std::string& response, ResponseType type);
};

} // namespace redis

