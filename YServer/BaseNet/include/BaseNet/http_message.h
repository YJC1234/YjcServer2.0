#pragma once

#include <BaseNet/socket.h>
#include <BaseNet/task.h>
#include <spdlog/spdlog.h>
#include <format>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace YServer {

class client_socket;

//方法 url  版本
//头部列表
//空行(/r/n)
// body
class http_request {
public:
    using ptr = std::shared_ptr<http_request>;

    std::string                                       method;
    std::string                                       url;
    std::string                                       version;
    std::vector<std::tuple<std::string, std::string>> header_list;
};

//版本 状态 状态文本
//头部列表
//空行(/r/n)
// body
class http_response {
public:
    using ptr = std::shared_ptr<http_response>;

    std::string                                       version_;
    std::string                                       status_;
    std::string                                       status_text_;
    std::vector<std::tuple<std::string, std::string>> header_list_;

    http_response();

    [[nodiscard]] std::string serialize() const;

    void set_socket(std::shared_ptr<client_socket> client_socket);

    void set_status(int code);

    void set_version(const std::string& version);

    void set_status_text(const std::string& text);

    void set_Header(const std::string& key, const std::string& value);

    template <class... Args>
    void send(const std::format_string<Args...> fmt, Args&&... args) {
        std::string body = std::format(fmt, std::forward<Args>(args)...);
        size_t      len = body.size();
        set_Header("Content-Length", std::to_string(len));
        std::string* message =  //!使用std::string会析构！应该由send释放message
            new std::string(serialize() + std::move(body));

        auto send_task = client_socket_->send(*message, message->size());
        send_task.resume();
        send_task.detach();
    }

    void send(const std::string& body) {
        size_t len = body.size();
        set_Header("Content-Length", std::to_string(len));
        std::string* message =  //!使用std::string会析构！应该由send释放message
            new std::string(serialize() + std::move(body));

        auto send_task = client_socket_->send(*message, message->size());
        send_task.resume();
        send_task.detach();
    }

private:
    std::shared_ptr<client_socket> client_socket_;
};

}  // namespace YServer