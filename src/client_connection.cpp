#include "redis/client_connection.hpp"
#include "redis/database.hpp"
#include "redis/protocol.hpp"
#include <format>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

#include <spdlog/spdlog.h>

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
                spdlog::debug("Finished reading from socket {}", socket_fd_);
                break;
            }
            if (errno == ECONNRESET) {
                spdlog::debug("Client socket {} closed by peer", socket_fd_);
                close();
                break;
            }
            throw std::runtime_error(std::format("Failed to read from socket: {}", strerror(errno)));
        } else if (bytes_read == 0) {
            spdlog::debug("Client socket {} closed by peer", socket_fd_);
            close();
            break;
        }
        spdlog::debug("Read {} bytes from socket {}", bytes_read, socket_fd_);
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
                spdlog::debug("Socket {} is not ready for writing", socket_fd_);
                break;
            }
            throw std::runtime_error(std::format("Failed to write to socket: {}", strerror(errno)));
        }
        spdlog::debug("Wrote {} bytes to socket {}", result, socket_fd_);
        response_queue_.pop();
    }
}

bool ClientConnection::hasPendingData() const {
    return !response_queue_.empty();
}

} // namespace redis

