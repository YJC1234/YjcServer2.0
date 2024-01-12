#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace YServer {

class http_server;
class http_request;
class http_response;

using HandlerFunc = std::function<void(http_request::ptr, http_response::ptr)>;

class Engine {
public:
    Engine();

    void ServeHTTP(http_request::ptr request, http_response::ptr response);
    void Run(const std::string& port);

    void Get(const std::string& path, HandlerFunc& handler);
    void Post(const std::string& path, HandlerFunc& handler);

private:
    void addRoute(const std::string& method, const std::string& path,
                  HandlerFunc& handler);

    std::unique_ptr<http_server>                 server_;
    std::unordered_map<std::string, HandlerFunc> router_;
};

}  // namespace YServer
