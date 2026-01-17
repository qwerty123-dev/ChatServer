// src/chatserver/infrastructure/repository/postgres_message_repository.cpp
#include "chatserver/infrastructure/repository/postgres_message_repository.h"

#include "chatserver/domain/message/message.h"
#include "chatserver/domain/message/message_text.h"
#include "chatserver/domain/user/user_id.h"

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <stdexcept>

namespace chatserver::infrastructure::repository {

// Helper to mask password in connection string for safe logging
static std::string mask_connstr(const std::string& s) {
    std::string out = s;
    const std::string key = "password=";
    auto pos = out.find(key);
    if (pos != std::string::npos) {
        auto start = pos + key.size();
        out.replace(start, std::string::npos, "***");
    }
    return out;
}

PostgresMessageRepository::PostgresMessageRepository(const std::string& connStr)
    : connStr_(connStr) {}

std::int64_t PostgresMessageRepository::save(
    const chatserver::domain::message::Message& message
) {
    try {
        pqxx::connection conn(connStr_);
        if (!conn.is_open()) {
            std::cerr << "[PostgresMessageRepository::save] PQ connection failed: connstr=["
                      << mask_connstr(connStr_) << "]" << std::endl;
            throw std::runtime_error("failed to open database connection");
        }

        pqxx::work txn(conn);

        // SQL: только sender_id и text (и, если в БД есть created_at с DEFAULT now(), не передаём его)
        pqxx::result result = txn.exec_params(
            "INSERT INTO messages (sender_id, text) VALUES ($1, $2) RETURNING id",
            message.sender_id().value(),
            message.text().value()
        );

        txn.commit();

        if (result.empty()) {
            std::cerr << "[PostgresMessageRepository::save] INSERT returned no rows" << std::endl;
            throw std::runtime_error("insert returned no id");
        }

        return result[0][0].as<long long>();
    } catch (const std::exception& ex) {
        std::cerr << "[PostgresMessageRepository::save] ERROR: " << ex.what()
                  << " connstr=[" << mask_connstr(connStr_) << "]" << std::endl;
        throw;
    }
}

} // namespace chatserver::infrastructure::repository

