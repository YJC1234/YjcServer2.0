#pragma once

#include <condition_variable>
#include <coroutine>
#include <list>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>

namespace YServer {
class thread_pool {
private:
    std::stop_source                    m_stop_source;
    std::list<std::jthread>             m_threads;
    std::mutex                          m_mutex;
    std::condition_variable             m_cv;
    std::queue<std::coroutine_handle<>> m_coroutine_queue;

    void thread_loop();
    void enqueue(std::coroutine_handle<> handle);

public:
    explicit thread_pool(size_t thread_count);

    ~thread_pool();

    class schedule_awaiter {
    private:
        thread_pool& m_thread_pool;

    public:
        explicit schedule_awaiter(thread_pool& thread_pool)
            : m_thread_pool(thread_pool) {}

        bool await_ready() const noexcept {
            return false;
        }

        void await_suspend(std::coroutine_handle<> calling_handle) noexcept {
            m_thread_pool.enqueue(calling_handle);
        }

        void await_resume() noexcept {}
    };

    schedule_awaiter schedule();

    size_t size();
};
}  // namespace YServer