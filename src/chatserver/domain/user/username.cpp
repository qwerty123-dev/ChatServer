#include "chatserver/domain/user/username.h"
// Подключаем заголовочный файл для реализации методов.

#include <stdexcept>
// std::invalid_argument используется для выбрасывания исключения, 
// если имя пользователя некорректно.

namespace chatserver::domain {
// Пространство имен domain - часть DDD- структуры проекта, 
// где находится Value Object и сущности предметной области.

Username::Username(std::string value)
    : value_(std::move(value)) {
    // Конструктор принимает строку по значению, затем перемещает её в value_.
    // Такой подход оптимален: позволяет избежать лишнего копирования,
    // а также принимать как lvalue, так и rvalue строки.
    // std::move(value) гарантирует, что строка будет перемещена,
    // что особенно эффективно при больших строках.
    if (value_.empty()) {
        throw std::invalid_argument("Username cannot be empty");
    }
    // Простая валидация: имя пользователя не может быть пустым.
    // Если строка пустая — бросаем исключение std::invalid_argument.
    // Это делает Username надёжным Value Object: он всегда валиден.

}

const std::string& Username::value() const {
    return value_;
}
// Геттер, возвращающий ссылку на внутреннюю строку.
// Возвращение const std::string& позволяет избежать копирования
// и гарантирует, что вызывающий код не сможет изменить значение.
}

