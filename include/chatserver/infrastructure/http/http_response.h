#pragma once

#include <string>
#include <unordered_map>

namespace chatserver::infrastructure::http {
// Пространство имён инфраструктурного слоя, отвечающего за HTTP.
// Домен ничего не знает о HTTP - это правильно по DDD.
struct HttpResponse {
    int status_code = 200;
    // HTTP-статус ответа. По умолчанию 200 ОК.
    // Может быть 404, 500, 302 и т.д
    std::string body;
    // Тело HTTP-ответа: JSON, HTML, текст или бинарные данные.
    // Сервер заполняет это поле перед отправкой клиенту.
    std::unordered_map<std::string, std::string> headers;
    // Коллекция HTTP‑заголовков: "Content-Type", "Set-Cookie", "Location" и т.д.
    // unordered_map обеспечивает быстрый доступ по имени заголовка.
};

}

