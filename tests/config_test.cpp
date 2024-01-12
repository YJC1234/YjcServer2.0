#include <Config/Config.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace YServer;

auto g = Config::Lookup<std::string>("caca", "dinner", "123");

int main() {
    auto system_logger = spdlog::basic_logger_mt(
        "system_logger", "/home/yjc/yjcServer/logs/system.log");
    spdlog::info("name = {}, value = {}", g->getName(), g->getValue());
}