#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace YServer {

class Context;

using HandlerFunc = std::function<void(Context::ptr)>;

class Router {
public:
    using ptr = std::shared_ptr<Router>;

private:
    std::unordered_map<std::string, HandlerFunc> handlers_;

public:
    void addRoute(const std::string& method, const std::string& path,
                  const HandlerFunc& handler);

    void handle(std::shared_ptr<Context> ctx);
};

}  // namespace YServer
