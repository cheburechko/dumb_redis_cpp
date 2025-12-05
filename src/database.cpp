#include "redis/database.hpp"
#include "redis/protocol.hpp"

namespace redis {

Database::Database() {
}

Database::~Database() {
}

std::string Database::executeCommand(const CommandArgs& args) {
    return Protocol::serializeError("Invalid command");
}

Storage& Database::getStorage() {
    return storage_;
}

const Storage& Database::getStorage() const {
    return storage_;
}

std::string Database::handleSet(const CommandArgs& args) {
    // TODO: Implement SET command
    return "";
}

std::string Database::handleGet(const CommandArgs& args) {
    // TODO: Implement GET command
    return "";
}

std::string Database::handleDel(const CommandArgs& args) {
    // TODO: Implement DEL command
    return "";
}

std::string Database::handleExists(const CommandArgs& args) {
    // TODO: Implement EXISTS command
    return "";
}

std::string Database::handleUnknown(const std::string& command) {
    // TODO: Implement unknown command error
    return "";
}

} // namespace redis

