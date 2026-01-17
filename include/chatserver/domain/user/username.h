#pragma once

#include <string>
// std::string, так как Username хранит строковое значение.
namespace chatserver::domain {
// Пространство имен для доменных сущностей. Username - Value Object, 
// представляющий имя пользователя в предметной области.

class Username {
// Класс инкапсулирует имя пользователя как неизменяемое значение (Value Object).
public:
    explicit Username(std::string value);
    // Конструктор принимает строку. explicit предотвращает неявные преобразования
    // из std::string в Username, что повышает типобезопасность.
    const std::string& value() const;
    // Геттер, возвращающий ссылку на внутреннюю строку.
    // Возвращение const std::string& исключает копирование и защищает от изменений.

private:
    std::string value_;
    // Внутреннее хранилище имени пользователя.
    // Используется std::string, так как имя - текстовое значение.
};

}

