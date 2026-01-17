#pragma once

#include <string>
#include "chatserver/bootstrap/app_context.h"

namespace chatserver::bootstrap {

AppContext initialize_app(const std::string& dbConnStr,
                          const std::string& secret,
                          const std::string& address,
                          int port);

void run_app(const std::string& dbConnStr,
             const std::string& secret,
             const std::string& address,
             int port);

} // namespace chatserver::bootstrap

