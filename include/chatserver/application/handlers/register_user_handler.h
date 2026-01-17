#pragma once

#include <memory>
#include <cstdint>
#include <string>

#include "chatserver/application/commands/register_user_command.h"
// RegistereUserCommand - объект application-слоя, описывающий намерение
// зарегестрировать нового пользователя.
#include "chatserver/infrastructure/repository/user_repository.h"
// UserRepository - интерфейс доступа к данным о пользователях.
// Он находится в infrastructure, потому что знает о БД.
#include "chatserver/domain/services/password_hasher.h"
// PasswordHasher - доменный сервис, отвечающий за хэширование паролей.
// Это часть доменной логики, а не инфраструктуры.
namespace chatserver::application {
// RegisterUserHandler - это обработчик use-case "Register User".
// Он находится в application-слое и отвечает за:
// • преобразование данных команды в доменные объекты,
// • вызов доменных сервисов,
// • вызов репозитория,
// • возврат результата
// Он НЕ знает о HTTP, JSON, SQL - только о бизнес-процессе.

class RegisterUserHandler {
public:
    RegisterUserHandler(
        std::shared_ptr<domain::services::PasswordHasher> passwordHasher,
        std::shared_ptr<infrastructure::repository::UserRepository> userRepository
    );
    // Внедрение зависимостей (Dependency Injection):
    //   • PasswordHasher — доменный сервис
    //   • UserRepository — инфраструктурный репозиторий
    //
    // Handler сам ничего не создаёт — ему всё дают извне.
    // Это делает код тестируемым и независимым от инфраструктуры.

    // Выполняет регистрацию пользователя. Бросает исключение в случае ошибки.
    std::int64_t handle(const RegisterUserCommand& command);
    // Метод use-case:
    //   • принимает RegisterUserCommand (username + password),
    //   • проверяет, что пользователь не существует,
    //   • хэширует пароль,
    //   • создаёт доменную сущность User
    //   • сохраняет её через репозиторий,
    //   • возвращает ID нового пользователя.

private:
    std::shared_ptr<domain::services::PasswordHasher> passwordHasher_;
    // Доменный сервис для хэширования пароля.
    // Handler не знает, какой алгоритм используется — bcrypt, argon2, scrypt —
    // это скрыто за интерфейсом PasswordHasher.

    std::shared_ptr<infrastructure::repository::UserRepository> userRepository_;
    // Репозиторий для работы с пользователями.
    // Handler не знает SQL, таблицы, соединения — это скрыто в реализации.
};

}

