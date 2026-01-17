#pragma once

#include <string>
// Подключаем std::string, так как интерфейс работает с паролями в виде строк
#include "chatserver/domain/user/password_hash.h"
// Подключаем Value Object PasswordHash - типобезопасное представление хэша пароля.
namespace chatserver::domain::services {
// Пространство имён services - слой доменных сервисов (DDD).
// Здесь располагаются интерфейсы, определяющие поведение,
// которое не принадлежит сущностям или Value Object'ам.
class PasswordHasher {
// Абстрактный доменный сервис для хэширования паролей.
// Он определяет контракт, но не содержит реализации.
// Конкретные реализации (например, bcrypt, Argon2, PBKDF2)
// будут находится в инфраструктурном слое
public:
    virtual ~PasswordHasher() = default;
    // Виртуальный деструктор по умолчанию
    // Нужен для корректного удаления объектов через указатель на базовый класс.
    virtual PasswordHash hash(const std::string& plainPassword) const = 0;
    // Чисто виртуальный метод.
    // Принимает открытый пароль и возвращает Value Object PasswordHash
    // Возвращает PasswordHash усиливает типобезопасность и гарантирует, 
    // что хэш всегда валиден (не пустой, корректно создан).
    virtual bool verify(
        const std::string& plainPassword,
        const PasswordHash& hash
    ) const = 0;
    // Чисто виртуальный метод. Проверяет, соответствует ли открытый пароль 
    // переданному хэшу. Возвращает true при совпадении. 
    // Такой контракт позволяет легко подменять реализацию в тестах.
};

}

