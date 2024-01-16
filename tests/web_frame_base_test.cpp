#include <WebFrame/Context.h>
#include <WebFrame/frameMain.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <memory>

using namespace YServer;

int main() {
    spdlog::stdout_color_mt("system_logger");

    Engine engine;

    auto func = [](Context::ptr ctx) {
        ctx->String(200, "<h1>Hello, World!</h1>");
    };
    engine.Get("/", func);
    engine.Run("8080");
}