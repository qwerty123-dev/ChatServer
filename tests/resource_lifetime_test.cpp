#include <gtest/gtest.h>
#include <memory>
#include "chatserver/infrastructure/http/http_router.h"
#include "chatserver/infrastructure/http/http_request.h"
#include "chatserver/infrastructure/http/http_response.h"

using namespace chatserver::infrastructure::http;

struct HeapResource {
    int counter = 7;
    HttpResponse handle_request(const HttpRequest& req) {
        return HttpResponse{200, std::to_string(counter)};
    }
};

TEST(ResourceLifetime, LambdaCapturesSharedPtrKeepsResourceAlive) {
    HttpRouter router;

    // Создаём ресурс в shared_ptr и регистрируем лямбду, которая захватывает shared_ptr по значению
    auto res = std::make_shared<HeapResource>();
    std::weak_ptr<HeapResource> weak = res;

    router.add_route("POST", "/register", [res](const HttpRequest& req) -> HttpResponse {
        return res->handle_request(req);
    });

    // Сбрасываем локальную ссылку — ресурс должен жить внутри лямбды
    res.reset();
    ASSERT_TRUE(weak.expired() == false); // если expired==false, ресурс ещё жив

    HttpRequest req;
    req.method = "POST";
    req.target = "/register";
    req.body = "{}";

    // Вызов не должен падать и должен вернуть корректный ответ
    EXPECT_NO_THROW({
        HttpResponse resp = router.route(req);
        EXPECT_EQ(resp.status_code, 200);
        EXPECT_EQ(resp.body, "7");
    });
}

