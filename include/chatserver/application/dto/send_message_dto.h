#pragma once

#include <cstdint>
#include <string>
// std::int64_t — удобный тип для идентификаторов.
// std::string — текст сообщения, пришедший от клиента.
namespace chatserver::application {
// SendMessageDTO - это транспорт объект (DTO), который: 
// • создается REST-контроллером после парсинга JSON, 
// • содержит "сырые" данные от клиента
// • НЕ выполняет валидацию
// • НЕ является доменной моделью
// • НЕ является командой
// • служит промежуточным слоем между HTTP и application-слоем
// 
// Поток данных выглядит так:
// HTTP JSON → SendMessageDTO → SendMessageCommand → Handler → Domain → Repository → DB
struct SendMessageDTO {
    std::int64_t sender_id;
    // Идентификатор отправителя, пришедший извне
    // Это еще не доменный UserId - просто число.
    std::int64_t receiver_id;
    // Идентификатор получателя.
    // В handler'e он будет превращен в доменный UserId.
    std::string text;
    // Текст сообщения в сыром виде.
    // В application-слое он будет преобразован в MessageText,
    // который уже проверяет инварианты (не пустой, корректный и т.д).
};

}

// DTO (Data Transfer Object) - это сырой транспортный объект, который 
// существует только на границе системы (обычно в REST - контроллере) и 
// служит единственной цели:принять данные от клиента в том виде, 
// в котором они пришли по HTTP. DTO - это буфер между внешним миром и твоим приложение, 
// который обеспечивает стабильный контракт API.

// [HTTP JSON]
//     ↓
// [SendMessageDTO]  ← транспортный слой
//     ↓
// [SendMessageCommand]  ← application слой
//     ↓
// [SendMessageHandler]  ← use-case
//     ↓
// [Message, UserId, MessageText]  ← domain слой
//     ↓
// [MessageRepository]  ← интерфейс
//     ↓
// [SQL / Mongo / Redis]  ← инфраструктура

