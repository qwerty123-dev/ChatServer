#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
// Подключаем инструменты: 
// • std::function - хранение обработчиков
// • std::type_index - ключ для сопоставления типа команды -> обработчик
// • std::unorderd_map - таблица обработчиков
namespace chatserver::application {
// CommanfBus - часть application-слоя: 
// • работает с командами (use-case level)
// • управляет маршрутизацией действий
// • связывает контроллеры/REST с доменными сервисами
class CommandBus {
public:
    template<typename Command>
    using Handler = std::function<void(const Command&)>;
    // Handler - тип обработчика для конкретной команды. 
    // Он принимает const Command& и ничего не возвращает. 
    // Это соответствует паттерну Command Handler: 
    // команда -> действие -> побочный эффект (создать сущность, вызвать доменный сервис)
    template<typename Command>
    void register_handler(Handler<Command> handler) {
        // Регистрируем обработчик для конкретного типа команды.
        // Используем typeid(Command) как ключ.
        handlers_[typeid(Command)] =
            [handler](const void* cmd) {
                // Оборачиваем handler в type-erased функцию.
                // cmd - это void*, поэтому нужно привести к правильному типу.
                handler(*static_cast<const Command*>(cmd));
                // static_cast безопасен, потому что мы гарантируем.
                // что dispatch() передаст сюда правильный тип.
            };
    }

    template<typename Command>
    void dispatch(const Command& command) const {
        // dispatch - отправка команды.
        // Это точка входа для контроллеров/REST/CLI
        auto it = handlers_.find(typeid(Command));
        if (it != handlers_.end()) {
            // Если обработчик найден - вызываем его.
            it->second(&command);
            // Если обработчика нет - команда просто игнорируется.
            // В реальном приложении лучше бросать исклюючение или логировать.
        }
    }

private:
    std::unordered_map<std::type_index, std::function<void(const void*)>> handlers_;
    // Таблица: 
    // ключ: type_index команды 
    // значение: type-erased функция, принимающая void* 
    // 
    // Это позволяет хранить обработчики разных типов в одном контейнере
};

}

