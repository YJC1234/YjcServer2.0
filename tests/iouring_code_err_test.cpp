#include <BaseNet/io_uring.h>
#include <BaseNet/socket.h>
#include <liburing.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

using namespace YServer;

int main() {
    spdlog::stdout_color_mt("system_logger");
    server_socket socket;
    socket.bind("8080");
    socket.listen();
    ::io_uring ring;
    io_uring_queue_init((size_t)32, &ring, 0);

    struct sqedata {
        int event;
        int res;
    };
    sqedata data;
    data.event = 1;
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_accept(sqe, socket.get_raw_file_descriptor(), nullptr,
                         nullptr, 0);
    io_uring_sqe_set_data(sqe, &data);
    char buf[2048];

    while (true) {
        io_uring_submit_and_wait(&ring, 1);
        io_uring_cqe* cqe;
        unsigned      head;
        io_uring_for_each_cqe(&ring, head, cqe) {
            auto datap = (sqedata*)io_uring_cqe_get_data(cqe);
            if (datap->event == 1) {
                int          client_fd = cqe->res;
                auto         sqe2 = io_uring_get_sqe(&ring);
                std::string* http = new std::string("HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nHello World"); //!不能用std::string!
                io_uring_prep_send(sqe2, client_fd, http->c_str(), http->size(),
                                   0);
                auto data2 = new sqedata{.event = 2, .res = client_fd};
                io_uring_sqe_set_data(sqe2, data2);
            }
            // } else if (datap->event == 2) {
            //     int         client_fd = datap->res;
            //     std::string http =
            //         "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nHello
            //         World";
            //     io_uring_sqe* sqe3 = io_uring_get_sqe(&ring);
            //     io_uring_prep_send(sqe3, client_fd, http.c_str(),
            //     http.size(),
            //                        0);
            //     auto data3 = new sqedata{.event = 3, .res = client_fd};
            //     io_uring_sqe_set_data(sqe3, data3);
            // }
            io_uring_cqe_seen(&ring, cqe);
        }
    }
}