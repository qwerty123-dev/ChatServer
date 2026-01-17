#include "chatserver/infrastructure/http/http_router.h"
#include <iostream>

namespace chatserver::infrastructure::http {

void HttpRouter::add_route(const std::string& method,
                           const std::string& path,
                           HandlerFunc handler)
{
    const std::string key = make_key(method, path);
    // Формируем уникальный ключ маршрута вида "GET /login".
    // Это позволяет хранить маршруты в обычной hash‑таблице.
    routes_[key] = std::move(handler);
    // Сохраняем обработчик в таблицу маршрутов.
    // std::move позволяет избежать лишнего копирования std::function.
}

HttpResponse HttpRouter::route(const HttpRequest& request) const
{
    const std::string key = make_key(request.method, request.target);
    // Формируем ключ для поиска обработчика по методу и пути запроса.
    auto it = routes_.find(key);
    if (it == routes_.end()) {
        // Маршрут не найден — вернём 404
        HttpResponse resp;
        resp.status_code = 404;
        resp.body = R"({"error":"not found"})";
        // Если маршрут найден — вызываем соответствующий обработчик.
        return resp;
    }

    // Вызов зарегистрированного обработчика
    return it->second(request);
}

std::string HttpRouter::make_key(const std::string& method,
                                 const std::string& path) const
{
    // Нормализуем HTTP‑метод к верхнему регистру.
    // Нормализуем метод к верхнему регистру (на случай, если где‑то придёт "post")
    std::string m = method;
    for (char& c : m) c = static_cast<char>(::toupper(static_cast<unsigned char>(c)));

    // Ключ маршрута — это "<METHOD> <PATH>", например "POST /users/login"
    return m + " " + path;
}

}

