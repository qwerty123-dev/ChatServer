#include <gtest/gtest.h>

#include "chatserver/infrastructure/http/http_router.h"
#include "chatserver/infrastructure/http/http_request.h"
#include "chatserver/infrastructure/http/http_response.h"

using namespace chatserver::infrastructure::http;

// Простая структура, имитирующая ресурс с методом-обработчиком.
// Мы будем создавать её на стеке, регистрировать лямбду, которая захватывает ссылку,
// затем уничтожать объект и вызывать зарегистрированный обработчик.
struct StackResource {
    int counter = 42;
    HttpResponse handle_request(const HttpRequest& req) {
        // Доступ к полю, чтобы вероятность UB/падения была выше при использовании после free.
        if (counter == 0) return HttpResponse{200, "zero"};
        return HttpResponse{200, std::to_string(counter)};
    }
};

TEST(DanglingResource, StackResourceCapturedByLambdaCausesCrash) {
    HttpRouter router;

    // Регистрация маршрута с лямбдой, которая захватывает ссылку на локальный объект.
    {
        StackResource res;
        // захватываем ссылку на res (dangling after scope)
        router.add_route("POST", "/register", [&res](const HttpRequest& req) -> HttpResponse {
            // При вызове после уничтожения res это UB — возможно падение.
            return res.handle_request(req);
        });
        // res уничтожится при выходе из этого блока — лямбда в роутере теперь содержит висячую ссылку.
    }

    // Формируем запрос
    HttpRequest req;
    req.method = "POST";
    req.target = "/register";
    req.body = "{\"username\":\"alice\",\"password\":\"secret\"}";

    // EXPECT_DEATH запускает тест в отдельном процессе и ожидает, что вызов приведёт к аварийному завершению.
    // Пустая строка для regex означает, что мы не проверяем текст ошибки, только факт падения.
    EXPECT_DEATH({
        // Вызов маршрута — должен привести к UB / падению, если лямбда действительно использует уничтоженный объект.
        router.route(req);
    }, "");
}

