#pragma once

#include "chatserver/application/handlers/send_message_handler.h"
#include "chatserver/infrastructure/http/http_router.h"
// Подключаем application‑слой (use case отправки сообщения) и HTTP‑роутер.
// MessageResource — это адаптер между HTTP и application‑логикой.

namespace chatserver::infrastructure::http::resources {
// Пространство имён для HTTP‑ресурсов (endpoints).
// Это инфраструктурный слой, который связывает HTTP с application‑слоем.

class MessageResource {
public:
    explicit MessageResource(std::shared_ptr<chatserver::application::SendMessageHandler> sendHandler);
    // Конструктор принимает обработчик use case "отправить сообщение".
    // Используем shared_ptr, чтобы ресурс мог безопасно хранить зависимость.

    void register_routes(chatserver::infrastructure::http::HttpRouter& router);
    // Регистрирует HTTP‑маршруты, связанные с сообщениями.
    // Например:
    //   POST /messages/send → sendHandler_
    // Здесь ресурс определяет, какой URL вызывает какой use case

private:
    std::shared_ptr<chatserver::application::SendMessageHandler> sendHandler_;
    // Обработчик отправки сообщения.
    // Хранится как зависимость, чтобы HTTP‑слой мог вызвать application‑логику.
};

}

