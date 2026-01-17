#pragma once
#include "request_router.h"
#include <string>

namespace chatserver::transport {

class Server {
public:
    Server(const std::string& address, int port, RequestRouter& router);
    void run(); // блокирующий запуск сервера
private:
    std::string address_;
    int port_;
    RequestRouter& router_;
    void handle_connection(int client_sock);
};

}

