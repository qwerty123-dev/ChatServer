#pragma once

#include "user_repository.h"
#include <pqxx/pqxx>
#include <memory>

namespace chatserver::infrastructure::repository {

class PostgresUserRepository final : public UserRepository {
public:
    explicit PostgresUserRepository(const std::string& connStr);

    std::int64_t save(const chatserver::domain::user::User& user) override;
    std::optional<chatserver::domain::user::User>
    find_by_username(const std::string& username) override;

private:
    std::string connStr_;
};

}

