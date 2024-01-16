#pragma once

#include <BaseNet/http_message.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace YServer {

class http_server;
class http_request;
class http_response;
class http_server;
class Router;
class Context;

using HandlerFunc = std::function<void(std::shared_ptr<Context>)>;

class Engine {
public:
    Engine();
    ~Engine();

    void ServeHTTP(http_request::ptr request, http_response::ptr response);
    void Run(const std::string& port);

    void Get(const std::string& path, const HandlerFunc& handler);
    void Post(const std::string& path, const HandlerFunc& handler);

private:
    void addRoute(const std::string& method, const std::string& path,
                  const HandlerFunc& handler);

    std::unique_ptr<http_server> server_;
    std::unique_ptr<Router>      router_;
};

}  // namespace YServer
