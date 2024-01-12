#pragma once

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

    std::string                                       version;
    std::string                                       status;
    std::string                                       status_text;
    std::vector<std::tuple<std::string, std::string>> header_list;

    [[nodiscard]] std::string serialize() const;

    void set_socket(std::shared_ptr<client_socket> client_socket);

private:
    std::shared_ptr<client_socket> client_socket_;
};

}  // namespace YServer