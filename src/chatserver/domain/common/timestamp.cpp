#include "chatserver/domain/common/timestamp.h"
// Подключаем заголовочный файл, где объявлен класс Timestamp.
// Это позволяет использовать его методы и определять их реализацию.

namespace chatserver::domain {
// Открываем пространство имен domain внутри chatserver,
// чтобы реализация соответствовала объяалению в .h файле.

Timestamp Timestamp::now() {
    // Реализация статического метода, создающего Timestamp с текущим временем.
    auto now = clock::now();
    // Получаем текущее время из system_clock (псевдоним clock).
    // now - это std::chrono::time_point.
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();
    // Преобразуем время с начала эпохи (time_since_epoch) в секунды.
    // duration_cast гарантирует точное преобразование к секундам.
    // count() возвращает целочисленное значение.
    return Timestamp(seconds);
    // Создаем и возвращаем новый Timestamp, используя количество секунд.
}

Timestamp::Timestamp(std::int64_t epochSeconds)
    : epochSeconds_(epochSeconds) {}
// Конструктор, инициализирующий внутреннее поле epochSeconds_.
// Используем список инициализации для эффективности.

std::int64_t Timestamp::epoch_seconds() const {
    return epochSeconds_;
}
// Геттер, возвращающий количество секунд от эпохи.
// const гарантирует, что метод не изменяет объект.

bool Timestamp::operator==(const Timestamp& other) const {
    return epochSeconds_ == other.epochSeconds_;
}
// Оператор сравнеия на равенство.
// Два Timestamp равны, если их значения секунд совпадают.

bool Timestamp::operator<(const Timestamp& other) const {
    return epochSeconds_ < other.epochSeconds_;
}
// Оператор "меньше", позволяющий сортировать Timestamp,
// использовать в std::map, std::set и других структурах.
}
