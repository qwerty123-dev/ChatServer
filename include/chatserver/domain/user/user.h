#pragma once

#include "chatserver/domain/user/user_id.h"
// Подключаем Value Object UserId - уникальные идентификатор пользователя.
#include "chatserver/domain/user/username.h"
// Подключаем Value Object Username - доменное имя пользователя.
#include "chatserver/domain/user/password_hash.h"
// Подключаем Value Object PasswordHash - безопасное хранилище хэша пароля.
namespace chatserver::domain::user {
// Пространство имен user внутри domain - логическая группировка сущностей, 
// относящихся к пользователю в предметной области.
class User {
// Класс User - доменная сущность (Entity).
// В отличие от Value Object, сущность имеет устойчивую идентичность (id_), 
// которая отличает одного пользователя от другого.
public:
    // Полный конструктор — для уже существующего пользователя (из БД). 
    // Например, при загрузке данных из репозитория.
    User(chatserver::domain::UserId id,
         Username username,
         PasswordHash passwordHash);
    // Все параметры передаются по значению, затем перемещаются в поля.
    // Это упрощает вызов конструктора и позволяет использовать move semantics.

    // Конструктор для нового пользователя (id = 0, БД присвоит реальный). 
    // id = 0 означает, что реальный идентификатор будет назначен БД.
    User(Username username,
         PasswordHash passwordHash)
        : id_(chatserver::domain::UserId(0)),
          username_(std::move(username)),
          passwordHash_(std::move(passwordHash)) {}
    // Используем список инициализации
    // id_ задается как временный UserId(0).
    // username_ и passwordHash_ перемещаются для эффективности.

    const chatserver::domain::UserId& id() const;
    // Геттер, возвращающий константную ссылку на объект класса UserId 
    // (идентификатор пользователя).
    // Возвращается const&, чтобы избежать копирования и защитить от изменений.

    const Username& username() const;
    // Геттер для имени пользователя.

    const PasswordHash& password_hash() const;
    // Геттер для хэша пароля.

private:
    chatserver::domain::UserId id_;
    // Уникальный идентификатор пользователя - определяет сущность.
    Username username_;
    // Имя пользователя - Value Object, всегда валидное.
    PasswordHash passwordHash_;
    // Хэш пароля - Value Object, гарантирующий корректность и непустоту.
};
}

