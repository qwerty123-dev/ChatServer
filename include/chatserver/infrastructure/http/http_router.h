#pragma once

#include "http_request.h"
#include "http_response.h"
// Подключаем структуры HttpRequest и HttpResponse, с которыми будет работать роутер.
#include <functional>
#include <string>
#include <unordered_map>
// Стандартные типы: строки, хеш‑таблица и std::function для хранения обработчиков.

namespace chatserver::infrastructure::http {
// Пространство имён инфраструктурного слоя HTTP. 
using HandlerFunc = std::function<HttpResponse(const HttpRequest&)>;
// Тип обработчика маршрута: принимает HttpRequest, возвращает HttpResponse.
// std::function позволяет хранить любые callable: лямбды, функции, методы.
class HttpRouter {
public:
    void add_route(const std::string& method,
                   const std::string& path,
                   HandlerFunc handler);
    // Регистрирует новый маршрут.
    // method — HTTP‑метод ("GET", "POST" и т.д.)
    // path — путь ("/login", "/messages", "/users/{id}" и т.д.)
    // handler — функция, которая будет вызвана при совпадении метода и пути.

    HttpResponse route(const HttpRequest& request) const;
    // Находит подходящий обработчик по request.method + request.target.
    // Если маршрут найден — вызывает handler и возвращает его результат.
    // Если нет — обычно должен вернуть 404 (реализация в .cpp).

private:
    std::unordered_map<std::string, HandlerFunc> routes_;
    // Хранилище всех маршрутов.
    // Ключ — строка вида "GET:/login" (см. make_key).
    // Значение — обработчик.
    std::string make_key(const std::string& method, const std::string& path) const;
    // Вспомогательная функция: собирает ключ для routes_.
    // Например: method="POST", path="/auth" → "POST:/auth".
};

}

