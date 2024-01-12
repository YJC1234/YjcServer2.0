#include <WebFrame/Context.h>

namespace YServer {

Context::ptr Context::New(http_request::ptr  request,
                          http_response::ptr response) {
    auto ctx = std::make_shared<Context>();
    ctx->Request_ = request;
    ctx->Response_ = response;
    ctx->Path_ = request->url;
    ctx->Method_ = request->method;
    return ctx;
}

}  // namespace YServer