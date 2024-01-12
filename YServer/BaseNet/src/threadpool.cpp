#include <BaseNet/threadpool.h>

namespace YServer {

thread_pool::thread_pool(size_t thread_count) {
    for (size_t i = 0; i < thread_count; i++) {
        m_threads.emplace_back([&]() { thread_loop(); });
    }
}

thread_pool::~thread_pool() {
    m_stop_source.request_stop();
    m_cv.notify_all();
}

thread_pool::schedule_awaiter thread_pool::schedule() {
    return schedule_awaiter{*this};
}

size_t thread_pool::size() {
    return m_threads.size();
}

void thread_pool::thread_loop() {
    while (!m_stop_source.stop_requested()) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]() {
            return !m_coroutine_queue.empty() || m_stop_source.stop_requested();
        });
        if (m_stop_source.stop_requested()) {
            return;
        }
        auto handle = m_coroutine_queue.front();
        m_coroutine_queue.pop();
        lock.unlock();
        handle.resume();
    }
}

void thread_pool::enqueue(std::coroutine_handle<> handle) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_coroutine_queue.push(handle);
    m_cv.notify_one();
}

}  // namespace YServer