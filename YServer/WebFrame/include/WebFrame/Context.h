#pragma once
#include <BaseNet/http_message.h>
#include <memory>
#include <string_view>

namespace YServer {

class Context {
public:
    using ptr = std::shared_ptr<Context>;

private:
    // origin objects
    http_request::ptr  Request_;
    http_response::ptr Response_;
    // request info
    std::string_view Method_;
    std::string_view Path_;
    // response info
    int Status_code_;

public:
    static Context::ptr New(http_request::ptr  request,
                            http_response::ptr response);
};

}  // namespace YServer
