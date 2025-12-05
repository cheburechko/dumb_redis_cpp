#include "redis/client_connection.hpp"
#include "redis/database.hpp"
#include "redis/protocol.hpp"
#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace redis {

// ClientConnection implementation
ClientConnection::ClientConnection(int socket_fd, Database& db)
    : socket_fd_(socket_fd), database_(db), active_(true) {
}

ClientConnection::~ClientConnection() {
    close();
}

void ClientConnection::handle() {
    auto commands = Protocol::parseCommand(readRequest());
    if (!active_) {
        return;
    }
    if (!commands.has_value()) {
        response_queue_.push(Protocol::serializeError(commands.error()));
    } else {
        for (const auto& command : commands.value()) {
            response_queue_.push(database_.executeCommand(command));
        }
    }
    sendResponse();
}

void ClientConnection::close() {
    if (active_) {
        ::close(socket_fd_);
        active_ = false;
    }
}

bool ClientConnection::isActive() const {
    return active_;
}

std::string ClientConnection::readRequest() {
    static char buffer[1024];
    while (true) {
        ssize_t bytes_read = ::read(socket_fd_, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            throw std::runtime_error("Failed to read from socket");
        } else if (bytes_read == 0) {
            close();
        }
        buffer_.write(buffer, bytes_read);
    }
    return buffer_.str();
}

void ClientConnection::sendResponse() {
    while (!response_queue_.empty()) {
        auto& reply = response_queue_.front();
        auto result = ::write(socket_fd_, reply.c_str(), reply.length());
        if (result == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
        }
        response_queue_.pop();
    }
}

bool ClientConnection::hasPendingData() const {
    return !response_queue_.empty();
}

} // namespace redis

