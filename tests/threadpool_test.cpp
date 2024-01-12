#include <BaseNet/task.h>
#include <BaseNet/threadpool.h>
#include <spdlog/spdlog.h>
#include <chrono>

using namespace YServer;

int main() {
    thread_pool pool(4);
    int         i = 0;

    auto taskfun = [&]() -> task<> {
        co_await pool.schedule();
        spdlog::info(i);
        i++;
    };
    auto task1 = taskfun();
    auto task2 = taskfun();
    auto task3 = taskfun();
    task1.resume();
    task2.resume();
    task3.resume();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
