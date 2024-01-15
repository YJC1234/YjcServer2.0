#include <WebFrame/frameMain.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <memory>

using namespace YServer;

int main() {
    spdlog::stdout_color_mt("system_logger");

    Engine engine;
    std::cout << syscall(SYS_gettid) << std::endl;

    auto func = [](http_request::ptr request, http_response::ptr response) {
        std::cout << syscall(SYS_gettid) << std::endl;
        response->send("Hello World!");
    };
    engine.Get("/1", func);
    engine.Run("8080");
}