#pragma once

#include <memory>
#include <cstdint>

#include "chatserver/application/commands/send_message_command.h"
// SendMessageCommand — объект application-слоя, описывающий намерение
// отправить сообщение (sender_id, receiver_id, text).
#include "chatserver/domain/services/message_encryptor.h"
// MessageEncryptor — доменный сервис.
// Он отвечает за шифрование текста сообщения перед сохранением.
// Это часть доменной логики, а не инфраструктуры.
#include "chatserver/infrastructure/repository/message_repository.h"
// MessageRepository — интерфейс доступа к сообщениям.
// Он находится в infrastructure, потому что знает о БД.

namespace chatserver::application {
// SendMessageHandler — обработчик use-case "Send Message".
// Он находится в application-слое и отвечает за:
//   • преобразование данных команды в доменные объекты,
//   • вызов доменных сервисов (шифрование),
//   • создание доменной сущности Message,
//   • сохранение её через репозиторий,
//   • возврат ID созданного сообщения.
// Он НЕ знает о HTTP, JSON, SQL — только о бизнес-процессе.
class SendMessageHandler {
public:
    SendMessageHandler(
        std::shared_ptr<domain::services::MessageEncryptor> encryptor,
        std::shared_ptr<infrastructure::repository::MessageRepository> messageRepository
    );
    // Внедрение зависимостей (Dependency Injection):
    //   • MessageEncryptor — доменный сервис шифрования
    //   • MessageRepository — инфраструктурный репозиторий
    // Handler сам ничего не создаёт — ему всё дают извне.
    // Это делает код тестируемым и независимым от инфраструктуры.

    std::int64_t handle(const SendMessageCommand& command);
    // Основной метод use-case:
    //   • принимает SendMessageCommand,
    //   • шифрует текст,
    //   • создаёт доменную сущность Message,
    //   • сохраняет её,
    //   • возвращает ID нового сообщения.

private:
    std::shared_ptr<domain::services::MessageEncryptor> encryptor_;
    // Доменный сервис для шифрования текста.
    // Handler не знает, какой алгоритм используется — AES, RSA, ChaCha20 —
    // это скрыто за интерфейсом MessageEncryptor.
    std::shared_ptr<infrastructure::repository::MessageRepository> messageRepository_;
    // Репозиторий для работы с сообщениями.
    // Handler не знает SQL, таблицы, соединения — это скрыто в реализации.
};

}

