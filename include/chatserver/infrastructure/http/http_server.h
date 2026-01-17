#pragma once

#include "http_router.h"
#include <memory>
#include <string>
// Подключаем HttpRouter и стандартные типы: строки и умные указатели.

namespace chatserver::infrastructure::http {
// Пространство имён инфраструктурного слоя HTTP.
// Домен ничего не знает о HTTP — это правильно по DDD.

class HttpServer {
public:
    HttpServer(const std::string& address,
               int port,
               std::shared_ptr<HttpRouter> router);
    // Конструктор HTTP‑сервера.
    // address — IP‑адрес, на котором сервер будет слушать (например, "0.0.0.0").
    // port — порт (например, 8080).
    // router — объект маршрутизатора, который будет обрабатывать входящие запросы.
    void run();
    // Запускает сервер.
    // Обычно включает:
    //   - создание сокета,
    //   - bind(address, port),
    //   - listen()
    //   - цикл accept() → чтение запроса → передача в router → отправка ответа.
    // Реализация находится в .cpp.

private:
    std::string                       address_;
    // Адрес, на котором сервер слушает входящие HTTP‑запросы.
    int                               port_;
    // Порт, на котором сервер принимает соединения.
    std::shared_ptr<HttpRouter>       router_;
    // Маршрутизатор, который определяет, какой обработчик вызвать для конкретного запроса.
    // shared_ptr позволяет разделять один роутер между несколькими компонентами.
};

}

