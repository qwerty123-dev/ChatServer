#include "chatserver/transport/server/request_router.h"

namespace chatserver::transport {

void RequestRouter::register_route(const std::string& method, const std::string& path, Handler handler) {
    routes_[route_key(method, path)] = handler;
}

HttpResponse RequestRouter::route(const HttpRequest& request) const {
    auto it = routes_.find(route_key(request.method, request.path));
    if(it != routes_.end()) {
        return it->second(request);
    }
    return {404, "Not Found"};
}

std::string RequestRouter::route_key(const std::string& method, const std::string& path) const {
    return method + ":" + path;
}

}

