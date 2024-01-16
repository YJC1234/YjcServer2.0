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

void Context::Status(int code) {
    Status_code_ = code;
    Response_->set_status(code);
}

void Context::setHeader(const std::string& key, const std::string& value) {
    Response_->set_Header(key, value);
}



}  // namespace YServer