#pragma once
#include <BaseNet/http_message.h>
#include <format>
#include <memory>
#include <string_view>

namespace YServer {

class Context {
public:
    using ptr = std::shared_ptr<Context>;

public:
    // origin objects
    http_request::ptr  Request_;
    http_response::ptr Response_;
    // request info
    std::string Method_;
    std::string Path_;
    // response info
    int Status_code_;

public:
    static Context::ptr New(http_request::ptr  request,
                            http_response::ptr response);

    void Status(int code);

    void setHeader(const std::string& key, const std::string& value);

    template <class... Args>
    void String(int code, std::format_string<Args...> fmt, Args&&... args) {
        Status(code);
        setHeader("Content-Type", "text/plain");
        Response_->send(std::format(fmt, std::forward<Args>(args)...));
    }

    void String(int code, const std::string& str) {
        Status(code);
        setHeader("Content-Type", "text/plain");
        Response_->send(str);
    }

    template <class... Args>
    void HTML(int code, std::format_string<Args...> fmt, Args&&... args) {
        Status(code);
        setHeader("Content-Type", "text/html");
        Response_->send(std::format(fmt, std::forward<Args>(args)...));
    }

    void HTML(int code, const std::string& str) {
        Status(code);
        setHeader("Content-Type", "text/html");
        Response_->send(str);
    }
};

}  // namespace YServer
