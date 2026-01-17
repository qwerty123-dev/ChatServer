#include "chatserver/infrastructure/repository/postgres_user_repository.h"

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>

#include "chatserver/domain/user/user.h"
#include "chatserver/domain/user/user_id.h"
#include "chatserver/domain/user/username.h"
#include "chatserver/domain/user/password_hash.h"

namespace chatserver::infrastructure::repository {

PostgresUserRepository::PostgresUserRepository(const std::string& connStr)
    : connStr_(connStr) {}

// Helper to mask password in connection string for safe logging
static std::string mask_connstr(const std::string& s) {
    std::string out = s;
    const std::string key = "password=";
    auto pos = out.find(key);
    if (pos != std::string::npos) {
        auto start = pos + key.size();
        // replace everything after password= with ***
        out.replace(start, std::string::npos, "***");
    }
    return out;
}

std::int64_t PostgresUserRepository::save(
    const chatserver::domain::user::User& user
) {
    try {
        pqxx::connection conn(connStr_);
        if (!conn.is_open()) {
            std::cerr << "[PostgresUserRepository::save] PQ connection failed: connstr=["
                      << mask_connstr(connStr_) << "]" << std::endl;
            throw std::runtime_error("failed to open database connection");
        }

        pqxx::work txn(conn);

        pqxx::params params{
            user.username().value(),
            user.password_hash().value()
        };

        pqxx::result result = txn.exec(
            "INSERT INTO users (username, password_hash) "
            "VALUES ($1, $2) RETURNING id",
            params
        );

        txn.commit();

        if (result.empty()) {
            std::cerr << "[PostgresUserRepository::save] INSERT returned no rows" << std::endl;
            throw std::runtime_error("insert returned no id");
        }

        return result[0][0].as<long long>();
    }
    catch (const std::exception& ex) {
        std::cerr << "[PostgresUserRepository::save] ERROR: "
                  << ex.what() << " connstr=[" << mask_connstr(connStr_) << "]" << std::endl;
        throw;
    }
}

std::optional<chatserver::domain::user::User>
PostgresUserRepository::find_by_username(const std::string& username) {
    try {
        pqxx::connection conn(connStr_);
        if (!conn.is_open()) {
            std::cerr << "[PostgresUserRepository::find_by_username] PQ connection failed: connstr=["
                      << mask_connstr(connStr_) << "]" << std::endl;
            return std::nullopt;
        }

        pqxx::work txn(conn);

        pqxx::params params{ username };

        pqxx::result r = txn.exec(
            "SELECT id, username, password_hash "
            "FROM users WHERE username=$1 LIMIT 1",
            params
        );

        if (r.empty())
            return std::nullopt;

        auto row = r[0];

        chatserver::domain::UserId id(row["id"].as<long long>());
        chatserver::domain::Username uname(row["username"].as<std::string>());
        chatserver::domain::PasswordHash pass(row["password_hash"].as<std::string>());

        return chatserver::domain::user::User(id, uname, pass);
    }
    catch (const std::exception& ex) {
        std::cerr << "[PostgresUserRepository::find_by_username] ERROR: "
                  << ex.what() << " connstr=[" << mask_connstr(connStr_) << "]" << std::endl;
        return std::nullopt;
    }
}

} // namespace chatserver::infrastructure::repository

