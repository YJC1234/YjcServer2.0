#pragma once

#include <unistd.h>

#include <compare>
#include <coroutine>
#include <filesystem>
#include <optional>
#include <tuple>

#include <BaseNet/io_uring.h>
#include <BaseNet/task.h>

namespace YServer {
class file_descriptor {
public:
    file_descriptor();

    explicit file_descriptor(int raw_file_descriptor);

    ~file_descriptor();

    file_descriptor(file_descriptor&& other) noexcept;

    file_descriptor& operator=(file_descriptor&& other) noexcept;

    file_descriptor(const file_descriptor& other) = delete;

    file_descriptor& operator=(const file_descriptor& other) = delete;

    std::strong_ordering operator<=>(const file_descriptor& other) const;

    [[nodiscard]] int get_raw_file_descriptor() const;

protected:
    std::optional<int> raw_file_descriptor_;
};

class splice_awaiter {
public:
    splice_awaiter(int raw_file_descriptor_in, int raw_file_descriptor_out,
                   size_t length);

    [[nodiscard]] bool    await_ready() const;
    void                  await_suspend(std::coroutine_handle<> coroutine);
    [[nodiscard]] ssize_t await_resume() const;

private:
    const int    raw_file_descriptor_in_;
    const int    raw_file_descriptor_out_;
    const size_t length_;
    sqe_data     sqe_data_;
};

task<ssize_t> splice(const file_descriptor& file_descriptor_in,
                     const file_descriptor& file_descriptor_out,
                     const size_t           length);

std::tuple<file_descriptor, file_descriptor> pipe();

file_descriptor open(const std::filesystem::path& path);
}  // namespace YServer
