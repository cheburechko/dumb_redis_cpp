#include "redis/database.hpp"
#include "redis/protocol.hpp"
#include <algorithm>
#include <format>
#include <functional>
#include <ranges>

namespace redis {

namespace {
    using CommandHandler = std::function<std::string(const CommandArgsSpan&, redis::Storage&)>;

    std::string handleSet(const CommandArgsSpan& args, redis::Storage& storage) {
        if (args.size() < 2) {
            return Protocol::serializeError("Invalid command arguments");
        }
        const std::string& key = args[0];
        const std::string& value = args[1];
        storage.set(key, value);
        return Protocol::serializeSimpleString("OK");
    }

    std::string handleGet(const CommandArgsSpan& args, redis::Storage& storage) {
        if (args.size() != 1) {
            return Protocol::serializeError("Invalid command arguments");
        }
        const std::string& key = args[0];
        auto result = storage.get(key);
        if (result.has_value()) {
            auto value = result.value();
            if (value.has_value()) {
                return Protocol::serializeBulkString(value.value());
            }
            return Protocol::serializeNullBulkString();
        }
        return Protocol::serializeError(result.error());
    }   

    std::string handleDel(const CommandArgsSpan& args, redis::Storage& storage) {
        if (args.empty()) {
            return Protocol::serializeError("Empty command arguments");
        }
        int count = 0;
        for (const auto& key : args) {
            if (storage.del(key)) {
                count++;
            }
        }
        return Protocol::serializeInteger(count);
    }

    std::string handleExists(const CommandArgsSpan& args, redis::Storage& storage) {
        if (args.empty()) {
            return Protocol::serializeError("Empty command arguments");
        }
        int count = 0;
        for (const auto& key : args) {
            if (storage.exists(key)) {
                count++;
            }
        }
        return Protocol::serializeInteger(count);
    }   

    std::string handlePing(const CommandArgsSpan& args, redis::Storage&) {
        if (args.empty()) {
            return Protocol::serializeSimpleString("PONG");
        }
        if (args.size() == 1) {
            return Protocol::serializeBulkString(args[0]);
        }
        return Protocol::serializeError("Invalid command arguments");
    }

    std::string handleHello(const CommandArgsSpan& args, redis::Storage&) {
        if (args.empty() || args[0].compare("2") == 0) {
            return Protocol::serializeArray({
                "server", "dump_redis_cpp",
                "proto", "2",
                "version", "0.1.0",
            });
        }
        return Protocol::serializeError("NOPROTO");
    }

    const std::unordered_map<std::string, CommandHandler> command_handlers = {
        {"SET", handleSet},
        {"GET", handleGet},
        {"DEL", handleDel},
        {"EXISTS", handleExists},
        {"PING", handlePing},
        {"HELLO", handleHello},
    };
}

Database::Database() {
}

Database::~Database() {
}

std::string Database::executeCommand(const CommandArgs& args) {
    if (args.empty()) {
        return Protocol::serializeError("Empty command");
    }
    const std::string& command = args[0];
    auto handler = command_handlers.find(command);
    if (handler == command_handlers.end()) {
        return Protocol::serializeError(std::format("Unknown command: {}", command));
    }
    return handler->second(std::ranges::drop_view(args, 1), storage_);
}

} // namespace redis

