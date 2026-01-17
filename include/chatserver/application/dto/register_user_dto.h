#pragma once

#include <string>

namespace chatserver::application {
// RegisterUserDTO - это транспортный объект (DTO), который: 
// • создается REST-контроллером после парсинга JSON,
// • содержит "сырые" данные от клиента, 
// • НЕ выполняет валидацию, 
// • НЕ является доменной моделью, 
// • НЕ является командой, 
// • служит промежуточным слоем между HTTP и application-слоем

// DTO -> Command -> Handler -> Domain -> Repository
//
// DTO - это простые данные, без логики и инвариантов.
struct RegisterUserDTO {
    std::string username;
    // Имя пользователя, введенное клиентом.
    // Здесь оно еще не проверено и не превращено в доменный Username.
    std::string password;
    // Пароль в открытом виде.
    // В application-слое он будет преобразован в RegistrerUserCommand,
    // а затем PAsswordHasher создаст PasswordHash
};

}

// [HTTP / REST]
//     ↓
// [RegisterUserDTO]
//     ↓
// [RegisterUserCommand]
//     ↓ dispatch()
// [CommandBus]
//     ↓
// [RegisterUserHandler]
//     ↓
// [Domain: Username, Password, PasswordHash, User]
//     ↓
// [UserRepository interface]
//     ↓
// [Infrastructure: PostgresUserRepository → SQL INSERT]
//     ↓
// [DB]