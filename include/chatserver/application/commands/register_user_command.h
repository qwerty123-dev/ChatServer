#pragma once

#include <string>

namespace chatserver::application {
// Команда RegisterUserCommand - часть application-слоя.
// Она описывает намерение внешнего клиента (REST, CLI, теста)
// зарегистрировать нового пользователя. 
// 
// Команда - это НЕ доменная сущность и НЕ Value Object.
// Это просто перенос данных (DTO уровня use-case), который: 
// • создается контроллером 
// • передается в CommandBus
// • обрабатывается соответствующим handler'ом
struct RegisterUserCommand {
    std::string username;
    // Имя пользователя, введенное клиентом. 
    // Это еще не доменный Username - просто строка.
    std::string password;
    // Пароль в открытом виде. 
    // В handler'е он будет передан PasswordHasher для получения PasswordHash.
};

}

