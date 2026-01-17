#pragma once

#include <memory>
#include <vector>

#include "chatserver/application/handlers/register_user_handler.h"
#include "chatserver/application/handlers/login_user_handler.h"
#include "chatserver/application/handlers/send_message_handler.h"
#include "chatserver/infrastructure/http/http_server.h"
#include "chatserver/infrastructure/http/http_router.h"

// Forward declarations для ресурсов (чтобы не тянуть их заголовки здесь)
namespace chatserver::infrastructure::http::resources {
    class UserResource;
    class MessageResource;
}

namespace chatserver::bootstrap {

struct AppContext {
    // Application handlers
    std::shared_ptr<chatserver::application::RegisterUserHandler> registerHandler;
    std::shared_ptr<chatserver::application::LoginUserHandler> loginHandler;
    std::shared_ptr<chatserver::application::SendMessageHandler> sendMessageHandler;

    // HTTP infra
    std::shared_ptr<chatserver::infrastructure::http::HttpRouter> router;
    std::shared_ptr<chatserver::infrastructure::http::HttpServer> server;

    // Typed resources (рекомендуется — явный тип, проще читать и отлаживать)
    std::shared_ptr<chatserver::infrastructure::http::resources::UserResource> userResource;
    std::shared_ptr<chatserver::infrastructure::http::resources::MessageResource> messageResource;

    // Дополнительный контейнер для хранения любых ресурсов (если нужно хранить разные типы)
    // Можно не использовать, если достаточно typed fields выше.
    std::vector<std::shared_ptr<void>> resources;
};

} // namespace chatserver::bootstrap

