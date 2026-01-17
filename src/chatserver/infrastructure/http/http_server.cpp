// src/chatserver/infrastructure/http/http_server.cpp
#include "chatserver/infrastructure/http/http_server.h"
#include "chatserver/infrastructure/http/http_request.h"
#include "chatserver/infrastructure/http/http_response.h"

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

namespace chatserver::infrastructure::http {

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using tcp       = net::ip::tcp;
// Удобные псевдонимы для Beast/Asio, чтобы код был короче и читабельнее.

HttpServer::HttpServer(const std::string& address,
                       int port,
                       std::shared_ptr<HttpRouter> router)
    : address_(address)
    , port_(port)
    , router_(std::move(router))
{}
// Конструктор HTTP‑сервера.
// address — IP, на котором слушаем (например, "0.0.0.0").
// port — порт (например, 8080).
// router — объект маршрутизатора, который будет обрабатывать запросы.

void HttpServer::run() {
    try {
        net::io_context ioc{1};
        // io_context — основной объект Asio, управляющий событиями ввода/вывода.
        // {1} означает один поток обработки событий.
        tcp::endpoint endpoint{
            net::ip::make_address(address_),
            // это функция Boost.Asio, которая перобразует строку с IP-адремом в объект boost::asio::ip::address
            static_cast<unsigned short>(port_)
        };
        // Создаём TCP‑endpoint из адреса и порта.

        tcp::acceptor acceptor{ioc, endpoint};
        // acceptor — объект, который принимает входящие TCP‑соединения.

        std::cout << "HttpServer listening on " << address_ << ":" << port_ << std::endl;

        for (;;) {
            // Основной цикл сервера: бесконечно принимаем соединения.
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            // Блокирующее ожидание входящего соединения.
            // После accept() у нас есть socket для общения с клиентом.
            std::thread([sock = std::move(socket), this]() mutable {
                // Для каждого соединения создаём отдельный поток.
                // В реальном продакшене лучше использовать thread pool.
                try {
                    beast::flat_buffer buffer;
                    http::request<http::string_body> req;
                    // Буфер и объект HTTP‑запроса Beast.

                    // Читаем один HTTP‑запрос
                    http::read(sock, buffer, req);

                    // Debug: логируем заголовки и тело запроса (временно)
                    std::cerr << "[HTTP] Request: " << req.method_string() << " " << req.target() << "\n";
                    for (auto const& field : req) {
                        std::cerr << "[HTTP] Header: " << std::string(field.name_string())
                                  << ": " << std::string(field.value()) << "\n";
                    }
                    std::cerr << "[HTTP] Body length: " << req.body().size() << "\n";
                    std::cerr << "[HTTP] Body raw: [" << req.body() << "]\n";

                    // Конвертация Beast → HttpRequest
                    HttpRequest hreq;
                    hreq.method = std::string(req.method_string());
                    hreq.target = std::string(req.target());
                    hreq.body   = req.body();

                    for (auto const& field : req) {
                        hreq.headers.emplace(
                            std::string(field.name_string()),
                            std::string(field.value())
                        );
                    }

                    // Маршрутизация
                    // Передаём запрос в роутер.
                    HttpResponse hresp;
                    try {
                        hresp = router_->route(hreq);
                    } catch (const std::exception& ex) {
                        // Если обработчик маршрута упал — возвращаем 500.
                        std::cerr << "Router exception: " << ex.what() << std::endl;
                        hresp.status_code = 500;
                        hresp.body = R"({"error":"internal server error"})";
                    }

                    // Конвертация HttpResponse → Beast response
                    http::response<http::string_body> res;
                    res.version(req.version());
                    res.result(static_cast<http::status>(hresp.status_code));
                    res.set(http::field::content_type, "application/json");

                    // Если у тебя в HttpResponse есть headers — можно проставить их здесь
                    for (const auto& [name, value] : hresp.headers) {
                        res.set(name, value);
                    }

                    res.body() = hresp.body;
                    res.prepare_payload();
                    // prepare_payload() автоматически выставляет Content-Length.

                    http::write(sock, res);
                    // Отправляем ответ клиенту.

                    // Закрываем соединение после одного запроса
                    beast::error_code ec;
                    sock.shutdown(tcp::socket::shutdown_send, ec);
                }
                catch (const std::exception& e) {
                    std::cerr << "HTTP connection error: " << e.what() << std::endl;
                }
            }).detach();
            // detach() — поток работает сам по себе.
            // В продакшене лучше использовать пул потоков, а не detach().
        }
    }
    catch (const std::exception& e) {
        std::cerr << "HTTP Server error: " << e.what() << std::endl;
    }
}

} // namespace chatserver::infrastructure::http

