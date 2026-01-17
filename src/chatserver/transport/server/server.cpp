#include "chatserver/transport/server/server.h"
#include "chatserver/transport/server/response_mapper.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>
namespace chatserver::transport {

Server::Server(const std::string& address, int port, RequestRouter& router)
    : address_(address), port_(port), router_(router) {}

void Server::run() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on " << address_ << ":" << port_ << std::endl;

    std::vector<std::thread> threads;
    while(true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        if(client_sock < 0) {
            perror("accept");
            continue;
        }
        threads.emplace_back([this, client_sock](){
            handle_connection(client_sock);
            close(client_sock);
        });
    }

    for(auto& t : threads) t.join();
}

void Server::handle_connection(int client_sock) {
    char buffer[4096] = {0};
    int valread = read(client_sock, buffer, sizeof(buffer));
    if(valread <= 0) return;

    // Простейший разбор запроса
    std::string req_str(buffer, valread);
    std::istringstream iss(req_str);
    std::string method, path, http_version;
    iss >> method >> path >> http_version;

    std::string body;
    auto body_pos = req_str.find("\r\n\r\n");
    if(body_pos != std::string::npos) {
        body = req_str.substr(body_pos + 4);
    }

    HttpRequest req{method, path, body};
    HttpResponse resp = router_.route(req);

    std::string response_str = ResponseMapper::to_http(resp);
    send(client_sock, response_str.c_str(), response_str.size(), 0);
}

}

