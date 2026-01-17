#pragma once

#include <string>
#include <unordered_map>
// Подключаем стандартные типы: строки и хэш-таблицу для заголовков HTTP.
namespace chatserver::infrastructure::http {
// Пространство имен инфраструктурного слоя, отвечающего за HTTP.
// Домен ничего не знает о HTTP - это правильно по DDD.

struct HttpRequest {
    std::string method;
    // HTTP-метод запроса: "GET", "POST", "PUT", "DELETE" и т.д
    // Хранится как строка, без enum - гибко, но менее безопасно.
    std::string target;
    // Путь и query-параметры: например "api/messages?limit=10"
    // Это то, что сервер должен маршрутизировать.
    std::string body;
    // Тело HTTP-запроса (JSON, текст, бинарные данные)
    // Для POST/PUT обычно содержит полезную нагрузку.
    std::unordered_map<std::string, std::string> headers;
    // Коллекция HTTP-заголовков: "Content-Type", "Authorization", "User-Agent" и т.д.
    // unordered_map обеспечивает быстрый доступ по имени заголовка.
};

}

