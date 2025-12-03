#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace redis {

// Command parser and router
class Command {
public:
    using CommandHandler = std::function<std::string(const CommandArgs&)>;
    
    Command();
    ~Command();
    
    // Parse command string into command name and arguments
    static std::pair<std::string, CommandArgs> parse(const std::string& input);
    
    // Register a command handler
    void registerHandler(const std::string& command, CommandHandler handler);
    
    // Execute a command
    std::string execute(const std::string& command, const CommandArgs& args);
    
    // Check if command exists
    bool exists(const std::string& command) const;
    
private:
    std::unordered_map<std::string, CommandHandler> handlers_;
};

} // namespace redis

