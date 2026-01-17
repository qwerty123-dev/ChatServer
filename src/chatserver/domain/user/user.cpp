#include "chatserver/domain/user/user.h"

namespace chatserver::domain::user {
// Пространство имен user внутри domain - логическая группировка 
// всех сущностей и объектов, связанных с пользователем.
User::User(chatserver::domain::UserId id,
           Username username,
           PasswordHash passwordHash)
    : id_(id),
      username_(std::move(username)),
      passwordHash_(std::move(passwordHash)) {}
// Полный конструктор сущности User. 
// Используется при загрузке пользователя из БД, когда id уже известен.
// id_(id) - копируем UserId. Он легкий (int64_t), копирование безопасно. 
// username_(std::move(username)) - перемещаем Username. Это Value Object, перемещение оптимально.
// passwordHash_(std::move(passwordHash)) - перемещаем PasswordHash. 
// Хэш может быть длинной строкой, поэтому move-семантика экономит ресурсы
const chatserver::domain::UserId& User::id() const {
    return id_;
}
// Геттер, возвращающий ссылку на идентификатор пользователя.
// Возвращаем ссылку на объект чтобы избежать копирования и не позволить 
// внешнему коду изменить id.
const Username& User::username() const {
    return username_;
}
// Геттер для имени пользователя.
// Возвращаем const&, так как Username - Value Object, и менять его извне нельзя.
const PasswordHash& User::password_hash() const {
    return passwordHash_;
}
// Геттер для хэша пароля.
// Возвращаем const&, чтобы не копировать строку и не позволять её менять.

}

