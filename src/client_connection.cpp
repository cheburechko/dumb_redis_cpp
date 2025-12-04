#include "redis/client_connection.hpp"
#include "redis/database.hpp"
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
            return;
        }
        buffer_.write(buffer, bytes_read);
    }
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
    // TODO: Implement reading from socket
    return "";
}

void ClientConnection::sendResponse(const std::string& response) {
    // TODO: Implement sending to socket
}

bool ClientConnection::hasPendingData() const {
    return false;
}

} // namespace redis

