#include <WebFrame/Context.h>
#include <WebFrame/Router.h>

namespace YServer {

void Router::addRoute(const std::string& method, const std::string& path,
                      const HandlerFunc& handler) {
    handlers_[method + "-" + path] = handler;
}

void Router::handle(std::shared_ptr<Context> ctx) {
    std::string key = ctx->Method_ + "-" + ctx->Path_;
    if (handlers_.count(key)) {
        handlers_[key](ctx);
    } else {
        ctx->String(404, "<h1>404 Not Found</h1>");
    }
}

}  // namespace YServer