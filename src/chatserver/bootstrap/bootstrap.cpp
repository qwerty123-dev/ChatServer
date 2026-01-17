#include "chatserver/bootstrap/bootstrap.h"

#include "chatserver/infrastructure/crypto/openssl_password_hasher.h"
#include "chatserver/infrastructure/crypto/openssl_message_encryptor.h"
#include "chatserver/infrastructure/repository/postgres_user_repository.h"
#include "chatserver/infrastructure/repository/postgres_message_repository.h"
#include "chatserver/application/handlers/register_user_handler.h"
#include "chatserver/application/handlers/login_user_handler.h"
#include "chatserver/application/handlers/send_message_handler.h"
#include "chatserver/infrastructure/http/resources/user_resource.h"
#include "chatserver/infrastructure/http/resources/message_resource.h"
#include "chatserver/infrastructure/http/http_server.h"
#include "chatserver/infrastructure/http/http_router.h"

namespace chatserver::bootstrap {

AppContext initialize_app(const std::string& dbConnStr,
                          const std::string& secret,
                          const std::string& address,
                          int port)
{
    // ---------------------
    // Crypto
    // ---------------------
    auto passwordHasher   = std::make_shared<infrastructure::crypto::OpenSSLPasswordHasher>();
    auto messageEncryptor = std::make_shared<infrastructure::crypto::OpenSSLMessageEncryptor>(secret);

    // ---------------------
    // Repositories
    // ---------------------
    auto userRepo    = std::make_shared<infrastructure::repository::PostgresUserRepository>(dbConnStr);
    auto messageRepo = std::make_shared<infrastructure::repository::PostgresMessageRepository>(dbConnStr);

    // ---------------------
    // Application Handlers
    // ---------------------
    auto registerHandler = std::make_shared<application::RegisterUserHandler>(
        passwordHasher,
        userRepo
    );

    auto loginHandler = std::make_shared<application::LoginUserHandler>(
        passwordHasher,
        userRepo
    );

    auto sendHandler = std::make_shared<application::SendMessageHandler>(
        messageEncryptor,
        messageRepo
    );

    // ---------------------
    // HTTP Router
    // ---------------------
    auto router = std::make_shared<infrastructure::http::HttpRouter>();

    // ---------------------
    // HTTP Resources (создаём как shared_ptr и сохраняем в контексте)
    // ---------------------
    auto userResource = std::make_shared<infrastructure::http::resources::UserResource>(
        registerHandler, loginHandler
    );

    auto messageResource = std::make_shared<infrastructure::http::resources::MessageResource>(
        sendHandler
    );

    // Регистрируем маршруты
    userResource->register_routes(*router);
    messageResource->register_routes(*router);

    // ---------------------
    // HTTP Server
    // ---------------------
    auto server = std::make_shared<infrastructure::http::HttpServer>(
        address,
        port,
        router
    );

    // ---------------------
    // Context
    // ---------------------
    AppContext ctx;
    ctx.registerHandler    = registerHandler;
    ctx.loginHandler       = loginHandler;
    ctx.sendMessageHandler = sendHandler;
    ctx.router             = router;
    ctx.server             = server;

    // Сохраняем ресурсы в контексте, чтобы их lifetime покрывал работу сервера
    ctx.userResource    = userResource;
    ctx.messageResource = messageResource;

    // Для совместимости/удобства можно также хранить их в контейнере void-указателей
    ctx.resources.emplace_back(std::static_pointer_cast<void>(userResource));
    ctx.resources.emplace_back(std::static_pointer_cast<void>(messageResource));

    return ctx;
}

void run_app(const std::string& dbConnStr,
             const std::string& secret,
             const std::string& address,
             int port)
{
    auto ctx = initialize_app(dbConnStr, secret, address, port);
    ctx.server->run();
}

} // namespace chatserver::bootstrap

