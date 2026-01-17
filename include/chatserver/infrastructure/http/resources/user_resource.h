#pragma once

#include "chatserver/application/handlers/register_user_handler.h"
#include "chatserver/application/handlers/login_user_handler.h"
#include "chatserver/infrastructure/http/http_router.h"
// Подключаем application‑слой (use cases / handlers) и HTTP‑роутер инфраструктуры.
// UserResource — это адаптер между HTTP и application‑слоем.

namespace chatserver::infrastructure::http::resources {
// Пространство имён для HTTP‑ресурсов (endpoints).
// Это инфраструктурный слой, который связывает HTTP с application‑логикой.

class UserResource {
public:
    UserResource(
        std::shared_ptr<chatserver::application::RegisterUserHandler> registerHandler,
        std::shared_ptr<chatserver::application::LoginUserHandler> loginHandler
    );
    // Конструктор принимает два обработчика application‑слоя:
    // 1) RegisterUserHandler — use case регистрации пользователя.
    // 2) LoginUserHandler — use case логина.
    // Используем shared_ptr, чтобы ресурс мог безопасно хранить ссылки на обработчики.

    void register_routes(chatserver::infrastructure::http::HttpRouter& router);
    // Метод, который регистрирует HTTP‑маршруты, связанные с пользователями.
    // Например:
    //   POST /users/register → registerHandler_
    //   POST /users/login    → loginHandler_
    // Здесь UserResource определяет, какие URL соответствуют каким use case.


private:
    std::shared_ptr<chatserver::application::RegisterUserHandler> registerHandler_;
    // Обработчик регистрации пользователя.
    // Хранится как зависимость, чтобы HTTP‑слой мог вызвать application‑логику.
    std::shared_ptr<chatserver::application::LoginUserHandler> loginHandler_;
    // Обработчик логина пользователя.
    // Также внедряется через конструктор.
};

}

