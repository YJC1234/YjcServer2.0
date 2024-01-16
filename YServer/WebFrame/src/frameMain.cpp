#include <BaseNet/http_message.h>
#include <BaseNet/http_server.h>
#include <WebFrame/Context.h>
#include <WebFrame/Router.h>
#include <WebFrame/frameMain.h>
#include <spdlog/spdlog.h>

namespace YServer {

Engine::Engine() {
    server_ = std::make_unique<http_server>(this);
}

Engine::~Engine() {}

void Engine::addRoute(const std::string& method, const std::string& path,
                      const HandlerFunc& handler) {
    router_->addRoute(method, path, handler);
}

void Engine::Get(const std::string& path, const HandlerFunc& handler) {
    addRoute("GET", path, handler);
}

void Engine::Post(const std::string& path, const HandlerFunc& handler) {
    addRoute("POST", path, handler);
}

void Engine::Run(const std::string& port) {
    server_->listen(port.c_str());
}

void Engine::ServeHTTP(http_request::ptr request, http_response::ptr response) {
    auto ctx = Context::New(request, response);
    router_->handle(ctx);
}
}  // namespace YServer
