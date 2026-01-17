#pragma once

#include <string>

namespace chatserver::domain {
// Пространство имен domain - слой предметной области (DDD).
// PasswordHash - Value Object, инкапсулирующий хэш пароля.

class PasswordHash {
// Класс представляет собой неизменяемый объект-значение (Value Object),
// который хранит хэш пароля как строку
public:
    explicit PasswordHash(std::string value);
    // Конструктор принимает строку с хэшем.
    // explicit предотвращает неявные преобразования из std::string в PasswordHash,
    // что повышает типобезопасность.

    const std::string& value() const;
    // Геттер, возвращающий ссылку на строку с хэшем.
    // const std::string& позволяет избежать копирования и защищает от изменений

private:
    std::string value_;
    // Внутреннее хранилище хэша пароля.
    // std::string подходит, так как хэш - это текстовое представление байтов.
};

}

