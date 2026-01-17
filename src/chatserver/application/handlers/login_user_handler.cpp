#include "chatserver/application/handlers/login_user_handler.h"
// Подключаем заголовок с объявлением LoginUserHandler
// Это реализация use-case "Login User" в application-слое

namespace chatserver::application {

LoginUserHandler::LoginUserHandler(
    std::shared_ptr<domain::services::PasswordHasher> passwordHasher,
    // PasswordHasher — доменный сервис.
    std::shared_ptr<infrastructure::repository::UserRepository> userRepository
    // UserRepository — инфраструктурный компонент.
)
    : passwordHasher_(std::move(passwordHasher))
    // Сохраняем сервис хэширования в приватное поле.
    , userRepository_(std::move(userRepository)) {}
    // Сохраняем репозиторий пользователей.

std::int64_t LoginUserHandler::handle(
    const LoginUserCommand& command
    // Команда application-слоя.
    // Она содержит "сырые" данные, пришедшие от контроллера:
    //   • username (string)
    //   • password (string)
    // Команда НЕ является доменной моделью и НЕ содержит логики.

) {
    auto user = userRepository_->find_by_username(command.username);
    // 1. Ищем пользователя по имени.
    // Репозиторий возвращает либо объект User, либо null/optional.
    // Это инфраструктурная операция (SQL SELECT), скрытая за интерфейсом.
    // Handler не знает, как именно происходит поиск — это правильно.
    if (!user) {
        // 2. Если пользователь не найден — логин невозможен.
        // Сейчас возвращается -1, что является "магическим значением".
        // Это работает, но архитектурно слабее, чем исключения.
        return -1;
    }

    if (!passwordHasher_->verify(command.password, user->password_hash())) {
        // 3. Проверяем пароль.
        // verify() — доменная логика, скрытая в PasswordHasher.
        // Handler не знает алгоритм (bcrypt, argon2, PBKDF2) — и не должен.
        // user->password_hash() — доменный объект PasswordHash.
        // command.password — сырой пароль, пришедший от клиента.
        return -1;
        // Неверный пароль → логин неуспешен.
        // Опять же, -1 — не лучший способ сигнализировать об ошибке.
    }

    return user->id().value();
    // 4. Успех: возвращаем ID пользователя.
    // user->id() — доменный объект UserId.
    // value() — извлекает примитив (int64_t), который удобно вернуть контроллеру.
}

}

// [HTTP JSON]
//     ↓
// LoginUserDTO
//     ↓
// LoginUserCommand
//     ↓
// CommandBus.dispatch()
//     ↓
// LoginUserHandler.handle()
//     ↓
// UserRepository.find_by_username()
//     ↓
// PasswordHasher.verify()
//     ↓
// return user_id