#include "chatserver/domain/user/password_hash.h"

#include <stdexcept>
// std::invalid_argument используется для генерации исключения, 
// если хэш пароля передан в некорректном виде
namespace chatserver::domain {
// Пространство имен domain - слой предметной области (DDD), 
// где находится Value Objects, такие как PasswordHash.

PasswordHash::PasswordHash(std::string value)
    : value_(std::move(value)) {
    // Конструктор принимает строку по значению, затем перемещает её в value_.
    // Такой подход оптимален: позволяет избежать лишнего копирования,
    // а также принимать как lvalue, так и rvalue строки.

    // std::move(value) гарантирует перемещение строки, что эффективно \
    // при работе с потенциально длинными хэшами.
    if (value_.empty()) {
        throw std::invalid_argument("Password hash cannot be empty");
    }
    // Простая, но важная валидация: хэш пароля не может быть пустым.
    // Пустой хэш - это всегда ошибка, поэтому выбрасывается исключение.
}

const std::string& PasswordHash::value() const {
    return value_;
}
// Геттер, возвращающий ссылку на внутреннюю строку. 
// Возвращает const std:;string& исключает копирование и защищает от изменений.

}

