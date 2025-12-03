#pragma once

#include "types.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

namespace redis {

// Forward declarations
class RedisString;

// Storage engine for Redis data structures
class Storage {
public:
    Storage();
    ~Storage();
    
    // String operations
    bool set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    bool exists(const std::string& key);

    // Utility operations
    size_t size() const;
    void clear();
    
private:
    std::unordered_map<std::string, RedisValue> data_;
    ValueType getValueType(const std::string& key) const;
};

} // namespace redis

