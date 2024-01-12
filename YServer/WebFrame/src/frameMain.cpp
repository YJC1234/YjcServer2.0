#include <BaseNet/http_message.h>
#include <BaseNet/http_server.h>
#include <WebFrame/frameMain.h>

namespace YServer {

Engine::Engine() {
    server_ = std::make_unique<http_server>(this);
}

void Engine::addRoute(const std::string& method, const std::string& path,
                      HandlerFunc& handler) {
    std::string key = method + "-" + path;
    router_[key] = handler;
}

void Engine::Get(const std::string& path, HandlerFunc& handler) {
    addRoute("GET", path, handler);
}

void Engine::Post(const std::string& path, HandlerFunc& handler) {
    addRoute("POST", path, handler);
}

void Engine::Run(const std::string& port) {
    server_->listen(port.c_str());
}

void Engine::ServeHTTP(http_request::ptr request, http_response::ptr response) {
    std::string key = request->method + "-" + request->url;
    if (router_.count(key)) {
        router_[key](request, response);
    } else {
        // TODO
    }
}
}  // namespace YServer
