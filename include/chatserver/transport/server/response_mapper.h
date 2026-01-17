#pragma once
#include <string>
#include "request_router.h"

namespace chatserver::transport {

class ResponseMapper {
public:
    static std::string to_http(const HttpResponse& response);
};

}

