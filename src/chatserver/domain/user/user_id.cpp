#include "chatserver/domain/user/user_id.h"

namespace chatserver::domain {
// Пространство имен domain - слой предметной области (DDD).
// Здесь находятся Value Objects, такие как UserId.

UserId::UserId(std::int64_t value)
    : value_(value) {}
// Конструктор принимает 64-битное целое число и сохраняет его во внутреннем поле.
// Использование списка инициализации - оптимальный способ задать значение члену класса.
std::int64_t UserId::value() const {
    return value_;
}
// Геттер, возвращающий идентификатор пользователя.
// Возвращается по значению, так как это примитивный тип и копирование дешевое.
bool UserId::operator==(const UserId& other) const {
    return value_ == other.value_;
}
// Оператор сравнения на равенство.
// Два UserId считаются равными, если их числовые значения совпадают.

bool UserId::operator<(const UserId& other) const {
    return value_ < other.value_;
}
// Оператор "меньше" позволяет использоваться UserId в отсортированных контейнерах
// (std::set, std::map) и сравнивать их по числовому значению.
}

