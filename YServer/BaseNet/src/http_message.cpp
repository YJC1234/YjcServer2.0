#include <BaseNet/http_message.h>

#include <sstream>
#include <string>

namespace YServer {

//序列化http_response
std::string http_response::serialize() const {
    std::stringstream raw_http_response;
    raw_http_response << version << ' ' << status << ' ' << status_text
                      << "\r\n";
    for (const auto& [k, v] : header_list) {
        raw_http_response << k << ':' << v << "\r\n";
    }
    raw_http_response << "\r\n";
    return raw_http_response.str();
}

void http_response::set_socket(std::shared_ptr<client_socket> client_socket) {
    client_socket_ = client_socket;
}

}  // namespace YServer