#pragma once

#include "storage.hpp"
#include "types.hpp"
#include <string>

namespace redis {

// Database layer that wraps storage and provides higher-level operations
class Database {
public:
    Database();
    ~Database();
    
    // Execute a command and return response
    std::string executeCommand(const CommandArgs& args);
    
private:
    Storage storage_;
};

} // namespace redis

