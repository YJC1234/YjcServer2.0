#include <BaseNet/http_message.h>
#include <BaseNet/socket.h>

#include <sstream>
#include <string>

namespace YServer {

http_response::http_response()
    : status_("200"), status_text_("OK"), version_("HTTP/1.0") {}

//序列化http_response
std::string http_response::serialize() const {
    std::stringstream raw_http_response;
    raw_http_response << version_ << ' ' << status_ << ' ' << status_text_
                      << "\r\n";
    for (const auto& [k, v] : header_list_) {
        raw_http_response << k << ':' << v << "\r\n";
    }
    raw_http_response << "\r\n";
    return raw_http_response.str();
}

void http_response::set_socket(std::shared_ptr<client_socket> client_socket) {
    client_socket_ = client_socket;
}

void http_response::set_status(int code) {
    status_ = std::to_string(code);
}

void http_response::set_status_text(const std::string& text) {
    status_text_ = text;
}

void http_response::set_version(const std::string& version) {
    version_ = version;
}

void http_response::set_Header(const std::string& key,
                               const std::string& value) {
    header_list_.emplace_back(std::move(key), std::move(value));
}

}  // namespace YServer