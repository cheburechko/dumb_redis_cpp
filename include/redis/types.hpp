#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace redis {

// Redis value types
enum class ValueType {
    STRING,
    LIST,
    SET,
    HASH,
    NONE
};

// Forward declarations
class RedisString;
class RedisList;
class RedisSet;
class RedisHash;

// Redis value variant
using RedisValue = std::variant<
    std::shared_ptr<RedisString>,
    std::shared_ptr<RedisList>,
    std::shared_ptr<RedisSet>,
    std::shared_ptr<RedisHash>
>;

// Command arguments
using CommandArgs = std::vector<std::string>;

// Response types
enum class ResponseType {
    SIMPLE_STRING,
    ERROR,
    INTEGER,
    BULK_STRING,
    ARRAY,
    NULL_BULK_STRING,
    NULL_ARRAY
};

} // namespace redis

