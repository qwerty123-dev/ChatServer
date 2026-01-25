#include "chatserver/domain/message/message_id.h"

namespace chatserver::domain {
// Пространство имен domain - слой предметной области (DDD).
MessageId::MessageId(std::int64_t value)
    : value_(value) {}

// Конструктор принимает 64-битное число и сохраняет его во внутреннем поле value_.
// Используем список инициализации — это самый эффективный способ задать значение.

std::int64_t MessageId::value() const {
    return value_;
}
// Геттер, возвращающий числовое значение идентификатора сообщения
// Возвращается по значению, так как это примитивный тип.

bool MessageId::operator==(const MessageId& other) const {
    return value_ == other.value_;
}
// Оператор сравнения на равенство.
// Два MessageId равны, если совпадают их числовые значения.

}

