#pragma once

#include <optional>
#include <span>
#include <string>

namespace YServer {
class http_request;

class http_parser {
public:
    std::optional<http_request> parse_packet(std::span<char> packet);

private:
    std::string raw_http_request_;
};
}  // namespace YServer
