#pragma once

#include <memory>
#include <cstdint>

#include "chatserver/application/commands/login_user_command.h"
// LoginUserCommand - объект application-слоя, описывающий намерение пользователя войти.
#include "chatserver/domain/services/password_hasher.h"
// PasswordHasher - доменный сервис, который умеет проверять пароль.
// Он находится в domain, потому что хэширование - часть бизнес-логики, 
// а не инфраструктуры.
#include "chatserver/infrastructure/repository/user_repository.h"
// UserRepository - интерфейс доступа к пользователям.
// Он находится в infrastructure, потому что знает о БД. 
namespace chatserver::application {
// LoginUserHandler - часть application-слоя.
// Его задача: принять команду, преобразовывать данные в доменные объекты,
// вызвать доменные сервисы и репозитории, вернуть результат.
// Он НЕ содержит бизнес-логики и НЕ знает о HTTP.
class LoginUserHandler {
public:
    LoginUserHandler(
        std::shared_ptr<domain::services::PasswordHasher> passwordHasher,
        std::shared_ptr<infrastructure::repository::UserRepository> userRepository
    );
    // Конструктор получает зависимости: 
    // • PasswordHasher - доменный сервис 
    // • UserRepository - инфраструктурный репозиторий
    // 
    // Это внедрение зависимостей (Dependency Injection)
    // Handler свм ничего не создает - ему всё дают извне.

    std::int64_t handle(const LoginUserCommand& command);
    // Основной метод use-case.
    // Принимает LoginUserCommand (username + password)
    // Возвращает ID пользователя (std::int64_t), если логин успешен.
    // Если логин неуспешен - handler должен бросить исклчение.

private:
    std::shared_ptr<domain::services::PasswordHasher> passwordHasher_;
    // Доменный сервис для проверки пароля.
    // Handler не знает, как устроено хэширование - он просто вызывает verify().
    std::shared_ptr<infrastructure::repository::UserRepository> userRepository_;
    // Репозитори для поиска пользователя по username.
    // Handler не знает SQL, таблицы, соединения - это скрыто в инфраструктуре.
};

}

