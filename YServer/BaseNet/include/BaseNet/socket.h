#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>

#include <cctype>
#include <chrono>
#include <coroutine>
#include <optional>
#include <span>
#include <tuple>

#include <BaseNet/file_descriptor.h>
#include <BaseNet/io_uring.h>
#include <BaseNet/task.h>

namespace YServer {

class server_socket : public file_descriptor {
public:
    server_socket();

    void bind(const char* port);

    void listen() const;

    class multishot_accept_guard {
    public:
        multishot_accept_guard(int               raw_file_descriptor,
                               sockaddr_storage* client_address,
                               socklen_t*        client_address_size);
        ~multishot_accept_guard();

        [[nodiscard]] auto await_ready() const -> bool;
        auto await_suspend(std::coroutine_handle<> coroutine) -> void;
        auto await_resume() -> int;

    private:
        bool              initial_await_ = true;
        const int         raw_file_descriptor_;
        sockaddr_storage* client_address_;
        socklen_t*        client_address_size_;
        sqe_data          sqe_data_;
    };

    auto accept(sockaddr_storage* client_address = nullptr,
                socklen_t*        client_address_size = nullptr)
        -> multishot_accept_guard&;

private:
    std::optional<multishot_accept_guard> multishot_accept_guard_;
};

class client_socket : public file_descriptor {
public:
    using ptr = std::shared_ptr<client_socket>;

    explicit client_socket(int raw_file_descriptor);

    class recv_awaiter {
    public:
        recv_awaiter(int raw_file_descriptor, size_t length);

        [[nodiscard]] bool await_ready() const;
        void               await_suspend(std::coroutine_handle<> coroutine);
        auto               await_resume() -> std::tuple<unsigned int, ssize_t>;

    private:
        const int    raw_file_descriptor_;
        const size_t length_;
        sqe_data     sqe_data_;
    };

    class recv_timeout_awaiter {
    public:
        recv_timeout_awaiter(int raw_file_descriptor, size_t length,
                             std::chrono::seconds duration_seconds);

        [[nodiscard]] bool await_ready() const;
        void               await_suspend(std::coroutine_handle<> coroutine);
        auto               await_resume() -> std::tuple<unsigned int, ssize_t>;

    private:
        const int            raw_file_descriptor_;
        const size_t         length_;
        std::chrono::seconds duration_seconds_;
        sqe_data             sqe_data_;
    };

    auto recv(size_t length) -> recv_awaiter;

    auto recv(size_t length, std::chrono::seconds duration_seconds)
        -> recv_timeout_awaiter;

    class send_awaiter {
    public:
        send_awaiter(int raw_file_descriptor, const std::span<char>& buffer,
                     size_t length);

        [[nodiscard]] auto await_ready() const -> bool;
        auto await_suspend(std::coroutine_handle<> coroutine) -> void;
        [[nodiscard]] auto await_resume() const -> ssize_t;

    private:
        const int              raw_file_descriptor_;
        const size_t           length_;
        const std::span<char>& buffer_;
        sqe_data               sqe_data_;
    };

    auto send(const std::span<char> buffer, size_t length) -> task<ssize_t>;
};

}  // namespace YServer

#endif