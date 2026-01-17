#pragma once

#include <memory>
#include <optional>
#include <string>
#include "chatserver/domain/user/user.h"

namespace chatserver::infrastructure::repository {

class UserRepository {
public:
    virtual ~UserRepository() = default;

    virtual std::int64_t save(const chatserver::domain::user::User& user) = 0;
    virtual std::optional<chatserver::domain::user::User>
    find_by_username(const std::string& username) = 0;
};

}

