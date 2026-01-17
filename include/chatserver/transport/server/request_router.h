#pragma once
#include <string>
#include <functional>
#include <unordered_map>

namespace chatserver::transport {

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
};

struct HttpResponse {
    int status_code;
    std::string body;
};

class RequestRouter {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    void register_route(const std::string& method, const std::string& path, Handler handler);
    HttpResponse route(const HttpRequest& request) const;

private:
    std::unordered_map<std::string, Handler> routes_;
    std::string route_key(const std::string& method, const std::string& path) const;
};

}

