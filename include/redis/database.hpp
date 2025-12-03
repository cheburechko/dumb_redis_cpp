#pragma once

#include "storage.hpp"
#include "types.hpp"
#include <string>
#include <optional>

namespace redis {

// Database layer that wraps storage and provides higher-level operations
class Database {
public:
    Database();
    ~Database();
    
    // Execute a command and return response
    std::string executeCommand(const std::string& command, const CommandArgs& args);
    
    // Get underlying storage (for direct access if needed)
    Storage& getStorage();
    const Storage& getStorage() const;
    
private:
    Storage storage_;
    
    // Command handlers
    std::string handleSet(const CommandArgs& args);
    std::string handleGet(const CommandArgs& args);
    std::string handleDel(const CommandArgs& args);
    std::string handleExists(const CommandArgs& args);
    std::string handleUnknown(const std::string& command);
};

} // namespace redis

