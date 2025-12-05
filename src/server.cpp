#include "redis/server.hpp"
#include "redis/client_connection.hpp"
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

const constexpr int MAX_EVENTS = 10;
const constexpr int TIMEOUT = 1000;

namespace {
    void set_nonblocking(int socket_fd) {
        int flags = fcntl(socket_fd, F_GETFL, 0);
        if (flags == -1) {
            throw std::runtime_error("Failed to get socket flags");
        }
        if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            throw std::runtime_error("Failed to set socket to non-blocking");
        }
    }

    int init_socket(const std::string& host, int port) {
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw std::runtime_error("Failed to create socket");
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = inet_addr(host.c_str());

        if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
            throw std::runtime_error("Failed to bind socket");
        }

        set_nonblocking(socket_fd);

        if (listen(socket_fd, SOMAXCONN) == -1) {
            throw std::runtime_error("Failed to listen on socket");
        }
        return socket_fd;
    }

    int init_epoll(int server_socket) {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = server_socket;

        int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            throw std::runtime_error("Failed to create epoll");
        }
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
            throw std::runtime_error("Failed to add server socket to epoll");
        }
        return epoll_fd;
    }
}

namespace redis {

// Server implementation
Server::Server(const std::string& host, int port)
    : host_(host), port_(port), server_socket_(-1), running_(false) {
}

Server::~Server() {
    stop();
}

void Server::start() {
    server_socket_ = init_socket(host_, port_);
    epoll_fd_ = init_epoll(server_socket_);
    running_ = true;
    epoll_event events[MAX_EVENTS];

    while (running_) {
        int nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, TIMEOUT);
        if (nfds == -1) {
            throw std::runtime_error("Failed to wait on epoll");
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_socket_) {
                acceptConnections();
            } else {
                handleClient(events[i].data.fd);
            }
        }
    }
    ::close(server_socket_);
    ::close(epoll_fd_);
}

void Server::stop() {
    // TODO: Implement server shutdown
    running_ = false;
}

bool Server::isRunning() const {
    return running_;
}

std::string Server::getHost() const {
    return host_;
}

int Server::getPort() const {
    return port_;
}

void Server::acceptConnections() {
    while (true) {
        sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            throw std::runtime_error("Failed to accept connection");
        }

        set_nonblocking(client_socket);

        connections_[client_socket] = std::make_unique<ClientConnection>(client_socket, database_);

        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = client_socket;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_socket, &event) == -1) {
            throw std::runtime_error("Failed to add client socket to epoll");
        }
    }
}

void Server::handleClient(int client_socket) {
    if (connections_.find(client_socket) == connections_.end()) {
        throw std::runtime_error("Client socket not found");
    }
    auto& connection = connections_[client_socket];
    auto had_pending_data = connection->hasPendingData();
    connection->handle();
    if (!connection->isActive()) {
        connections_.erase(client_socket);
        ::close(client_socket);
        return;
    }
    auto has_pending_data = connection->hasPendingData();
    if (had_pending_data != has_pending_data) {
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        if (has_pending_data) {
            event.events |= EPOLLOUT;
        }
        event.data.fd = client_socket;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, client_socket, &event) == -1) {
            throw std::runtime_error("Failed to modify client socket in epoll");
        }
    }
}

} // namespace redis

