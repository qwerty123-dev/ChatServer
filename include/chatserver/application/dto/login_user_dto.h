#pragma once

#include <string>
// std::string используется для передачи данных из внешнего мира.

namespace chatserver::application {
// LoginUserDTO - это объект уровня transport/application.
// Он представляет данные, пришедшие от клиента (например, из RESR-запроса),
// но еще НЕ является командой и НЕ является доменной моделью.
// 
// DTO - это чистый перенос данных (Data Transfer Object)
// • создается контроллером, 
// • используется для парсинга JSON,
// • затем преобразуется в LoginUserCommand, который уже передаётся в CommandBus
struct LoginUserDTO {
    std::string username;
    // Имя пользователя, введенное клиентом.
    // Здесь нет валидации - DTO не отвечает за инварианты.

    std::string password;
    // Пароль в открытом виде.
    // В application-слое он будет передан в LoginUserCommand,
    // а затем PasswordHasher проверит его корректность.
};

}

