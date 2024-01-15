#pragma once

#include <cstddef>
#include <functional>
#include <thread>

#include <BaseNet/socket.h>
#include <BaseNet/task.h>
#include <BaseNet/threadpool.h>

namespace YServer {

class Engine;

class thread_worker {
public:
    explicit thread_worker(const char* port, Engine* engine);

    auto accept_client() -> task<>;

    auto handle_client(client_socket::ptr client_socket) -> task<>;

    auto event_loop() -> task<>;

private:
    server_socket server_socket_;
    Engine*       engine_;
};

class http_server {
public:
    explicit http_server(
        Engine* engine,
        size_t  thread_count = std::thread::hardware_concurrency());

    void listen(const char* port);

private:
    thread_pool thread_pool_;
    Engine*     engine_;
};
}  // namespace YServer
