#include "chatserver/application/handlers/register_user_handler.h"
// Подключаем заголовок с объявлением RegisterUserHandler - обработчика use-case
// "Register User" в application-слое
#include <iostream>
#include <exception>

namespace chatserver::application {

RegisterUserHandler::RegisterUserHandler(
    std::shared_ptr<domain::services::PasswordHasher> passwordHasher,
    // PasswordHasher — доменный сервис, отвечающий за хэширование паролей.
    std::shared_ptr<infrastructure::repository::UserRepository> userRepository
    // UserRepository — инфраструктурный компонент, который знает о БД.
)
    : passwordHasher_(std::move(passwordHasher))
    // Сохраняем сервис хэширования в приватное поле.
    , userRepository_(std::move(userRepository)) {}
    // Сохраняем репозиторий пользователей.

std::int64_t RegisterUserHandler::handle(
    const RegisterUserCommand& command
    // RegisterUserCommand — объект application‑слоя.
    // Содержит "сырые" данные: username и password.
) {
    try {
        std::cout << "[RegisterUserHandler] handle start"
                  << " username='" << command.username << "'" << std::endl;
                  // Логируем начало обработки команды — полезно для отладки.

        if (!passwordHasher_) {
            // Проверяем, что зависимости корректно внедрены.
            // Это защита от ошибок конфигурации.
            std::cerr << "[RegisterUserHandler] ERROR: passwordHasher_ is null" << std::endl;
            throw std::runtime_error("passwordHasher not initialized");
        }
        if (!userRepository_) {
            // Аналогичная проверка для репозитория.
            std::cerr << "[RegisterUserHandler] ERROR: userRepository_ is null" << std::endl;
            throw std::runtime_error("userRepository not initialized");
        }

        std::cout << "[RegisterUserHandler] calling passwordHasher_->hash" << std::endl;
        auto hash = passwordHasher_->hash(command.password);
        // 1. Хэшируем пароль через доменный сервис.
        // Handler не знает алгоритм (bcrypt/argon2/etc.) — это скрыто за интерфейсом.
        std::cout << "[RegisterUserHandler] hash computed" << std::endl;

        chatserver::domain::user::User user(
            chatserver::domain::Username(command.username),
            // Преобразуем строку в доменный Value Object Username.
            // Здесь могут быть проверки инвариантов (не пустой, корректный формат).
            chatserver::domain::PasswordHash(hash)
            // Превращаем строку-хэш в доменный PasswordHash.
            // Это гарантирует, что доменная модель работает только с безопасными данными.
        );
        std::cout << "[RegisterUserHandler] user constructed" << std::endl;

        auto id = userRepository_->save(user);
        // 2. Сохраняем пользователя через репозиторий.
        // Handler не знает SQL — только вызывает интерфейс.
        // save() возвращает ID созданного пользователя.
        std::cout << "[RegisterUserHandler] save returned id=" << id << std::endl;

        return id;
        // 3. Возвращаем ID — результат успешной регистрации.
    }
    catch (const std::exception& ex) {
        // Ловим любые std::exception — логируем и пробрасываем дальше.
        // Это позволяет HTTP‑слою вернуть корректный статус (например, 500).
        std::cerr << "[RegisterUserHandler] EXCEPTION: " << ex.what() << std::endl;
        throw; // пробрасываем дальше, чтобы HTTP слой мог вернуть 500
    }
    catch (...) {
        // Ловим любые другие исключения (редко, но возможно).
        std::cerr << "[RegisterUserHandler] UNKNOWN EXCEPTION" << std::endl;
        throw;
    }
}

}

// ┌──────────────────────────────────────────────────────────────────────────┐
// │                           1. HTTP / REST слой                            │
// └──────────────────────────────────────────────────────────────────────────┘
//                 Клиент отправляет JSON:
//                 {
//                     "username": "john",
//                     "password": "secret"
//                 }
//                                    │
//                                    ▼
//                     Контроллер парсит JSON → RegisterUserDTO
//                                    │
//                                    ▼
//                    DTO → преобразуется в RegisterUserCommand
//                                    │
//                                    ▼
//                          commandBus.dispatch(command)
//                                    │
//                                    ▼

// ┌──────────────────────────────────────────────────────────────────────────┐
// │                         2. Application слой                              │
// └──────────────────────────────────────────────────────────────────────────┘
//                          CommandBus ищет handler
//                                    │
//                                    ▼
//                       RegisterUserHandler.handle(cmd)
//                                    │
//                                    ▼
//         Преобразование примитивов → Username, Password (Value Objects)
//                                    │
//                                    ▼
//                 passwordHasher.hash(password)  ← доменный сервис
//                                    │
//                                    ▼
//               Создание доменной сущности User(id, username, hash)
//                                    │
//                                    ▼
//                      userRepository.save(user)  ← интерфейс
//                                    │
//                                    ▼
//                          Возврат user_id контроллеру

// ┌──────────────────────────────────────────────────────────────────────────┐
// │                             3. Domain слой                               │
// └──────────────────────────────────────────────────────────────────────────┘
// Value Objects:
//     • Username
//     • Password
//     • PasswordHash
// Entity:
//     • User
// Service:
//     • PasswordHasher

// Domain ничего не знает о:
//     ✗ SQL
//     ✗ HTTP
//     ✗ JSON
//     ✗ CommandBus

// Только о бизнес‑логике.

//                                    │
//                                    ▼

// ┌──────────────────────────────────────────────────────────────────────────┐
// │                        4. Infrastructure слой                            │
// └──────────────────────────────────────────────────────────────────────────┘
// Реализация UserRepository:
//     • PostgresUserRepository
//     • MySQLUserRepository
//     • InMemoryUserRepository (для тестов)

// Метод save(user):
//     INSERT INTO users (id, username, password_hash) VALUES (...)

//                                    │
//                                    ▼

// ┌──────────────────────────────────────────────────────────────────────────┐
// │                                5. База данных                            │
// └──────────────────────────────────────────────────────────────────────────┘
// Таблица users:
//     id BIGINT PRIMARY KEY
//     username TEXT UNIQUE
//     password_hash TEXT

//                                    │
//                                    ▼

// ┌──────────────────────────────────────────────────────────────────────────┐
// │                           6. HTTP ответ клиенту                          │
// └──────────────────────────────────────────────────────────────────────────┘
// {
//     "status": "ok",
//     "user_id": 42
// }
