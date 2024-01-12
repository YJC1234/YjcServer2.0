#include <BaseNet/http_server.h>

#include <liburing.h>
#include <liburing/io_uring.h>

#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <BaseNet/buffer_ring.h>
#include <BaseNet/constant.h>
#include <BaseNet/file_descriptor.h>
#include <BaseNet/http_message.h>
#include <BaseNet/http_parser.h>
#include <BaseNet/io_uring.h>
#include <BaseNet/socket.h>
#include <BaseNet/sync_wait.h>
#include <WebFrame/frameMain.h>
#include <spdlog/spdlog.h>

namespace YServer {
thread_worker::thread_worker(const char* port, Engine* engine)
    : engine_(engine) {
    buffer_ring::get_instance().register_buffer_ring(BUFFER_RING_SIZE,
                                                     BUFFER_SIZE);
    server_socket_.bind(port);
    server_socket_.listen();

    task<> accept_client_task = accept_client();
    accept_client_task.resume();
    accept_client_task.detach();
}

auto thread_worker::accept_client() -> task<> {
    while (true) {
        const int raw_file_descriptor = co_await server_socket_.accept();
        if (raw_file_descriptor == -1) {
            continue;
        }

        task<> handle_client_task =
            handle_client(std::make_shared<client_socket>(raw_file_descriptor));
        handle_client_task.resume();
        handle_client_task.detach();
    }
}

task<> thread_worker::handle_client(client_socket::ptr client_socket) {
    http_parser  http_parser;
    buffer_ring& buffer_ring = buffer_ring::get_instance();
    while (true) {
        const auto [recv_buffer_id, recv_buffer_size] =
            co_await client_socket->recv(BUFFER_SIZE);
        if (recv_buffer_size == 0) {
            break;
        }

        const std::span<char> recv_buffer =
            buffer_ring.borrow_buffer(recv_buffer_id, recv_buffer_size);
        if (const auto parse_result = http_parser.parse_packet(recv_buffer);
            parse_result.has_value()) {
            auto request = std::make_shared<http_request>(parse_result.value());
            auto response = std::make_shared<http_response>();
            response->set_socket(client_socket);
            // TODO
            engine_->ServeHTTP(request, response);
            // const std::filesystem::path file_path =
            //     std::filesystem::relative(http_request.url, "/");

            // http_response http_response;
            // http_response.version = http_request.version;
            // if (std::filesystem::exists(file_path) &&
            //     std::filesystem::is_regular_file(file_path)) {
            //     http_response.status = "200";
            //     http_response.status_text = "OK";
            //     const uintmax_t file_size =
            //         std::filesystem::file_size(file_path);
            //     http_response.header_list.emplace_back(
            //         "content-length", std::to_string(file_size));

            //     std::string send_buffer = http_response.serialize();
            //     if (co_await client_socket.send(send_buffer,
            //                                     send_buffer.size()) == -1) {
            //         spdlog::get("system_logger")
            //             ->error("failed to invoke 'send'");
            //         throw std::runtime_error("failed to invoke 'send'");
            //     }

            //     const file_descriptor file_descriptor = open(file_path);
            //     if (co_await splice(file_descriptor, client_socket,
            //                         file_size) == -1) {
            //         spdlog::get("system_logger")
            //             ->error("failed to invoke 'splice'");
            //         throw std::runtime_error("failed to invoke 'splice'");
            //     }
            // } else {
            //     http_response.status = "404";
            //     http_response.status_text = "Not Found";
            //     http_response.header_list.emplace_back("content-length",
            //     "0");

            //     std::string send_buffer = http_response.serialize();
            //     if (co_await client_socket.send(send_buffer,
            //                                     send_buffer.size()) == -1) {
            //         spdlog::get("system_logger")
            //             ->error("failed to invoke 'send'");
            //         throw std::runtime_error("failed to invoke 'send'");
            //     }
            // }
        }

        buffer_ring.return_buffer(recv_buffer_id);
    }
}

auto thread_worker::event_loop() -> task<> {
    io_uring& io_uring = io_uring::get_instance();

    while (true) {
        io_uring.submit_and_wait(1);
        for (io_uring_cqe* const cqe : io_uring) {
            auto* sqe_data =
                reinterpret_cast<struct sqe_data*>(io_uring_cqe_get_data(cqe));
            sqe_data->cqe_res = cqe->res;
            sqe_data->cqe_flags = cqe->flags;
            void* const coroutine_address = sqe_data->coroutine;
            io_uring.cqe_seen(cqe);

            if (coroutine_address != nullptr) {
                std::coroutine_handle<>::from_address(coroutine_address)
                    .resume();
            }
        };
    }
}

http_server::http_server(Engine* engine, const size_t thread_count)
    : thread_pool_{thread_count}, engine_(engine) {}

void http_server::listen(const char* port) {
    const auto construct_task = [&]() -> task<> {
        co_await thread_pool_.schedule();
        co_await thread_worker(port, engine_).event_loop();
    };

    std::vector<task<>> thread_worker_list;
    for (size_t _ = 0; _ < thread_pool_.size(); ++_) {
        task<> thread_worker = construct_task();
        thread_worker.resume();
        thread_worker_list.emplace_back(std::move(thread_worker));
    }
    sync_wait_all(thread_worker_list);
}

}  // namespace YServer