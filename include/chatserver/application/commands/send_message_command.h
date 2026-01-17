#pragma once

#include <cstdint>
// std::int64_t — удобный тип для идентификаторов.
#include <string>
// std::string — текст сообщения, пришедший извне.

namespace chatserver::application {
// Команда SendMessageCommand - часть application-слоя. 
// Она описывает намерение клиента отправить сообщение. 
// 
// Это НЕ доменная сущность и НЕ Value Object.
// Это просто перенос данных (DTO уровня use-case), который:
// • создается REST-контроллером, 
// • передается в CommandBus 
// • обрабатывается соответствующим handler'ом 
// • преобразуется в доменные типы (UserId, MessageText, Timestamp).
struct SendMessageCommand {
    std::int64_t sender_id;
    // Идентификатор отправителя, пришедший извне.
    // Это еще не доменный UserId - просто число, полученное от клиента.
    std::string text;
    // Текст сообщения в сыром виде.
    // В handler'e он будет преобразован в MessageText,
    // который уже проверяет инварианты (не пустой, корректный и т.д).
};

}

// [REST] 
//    ↓
// [SendMessageCommand]
//    ↓ dispatch()
// [CommandBus]
//    ↓
// [SendMessageHandler]
//    ↓
// [Domain: Message, MessageId, MessageText]
//    ↓
// [MessageRepository]
//    ↓
// [Infrastructure: SQL / Mongo / Redis / etc.]