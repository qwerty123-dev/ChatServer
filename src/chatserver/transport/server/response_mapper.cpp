#include "chatserver/transport/server/response_mapper.h"
#include <sstream>

namespace chatserver::transport {

std::string ResponseMapper::to_http(const HttpResponse& response) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << response.status_code << " "
        << (response.status_code == 200 ? "OK" : "ERROR") << "\r\n";
    oss << "Content-Type: application/json\r\n";
    oss << "Content-Length: " << response.body.size() << "\r\n\r\n";
    oss << response.body;
    return oss.str();
}

}

