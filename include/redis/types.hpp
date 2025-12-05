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

// Redis value variant
using RedisValue = std::variant<
    std::string
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

