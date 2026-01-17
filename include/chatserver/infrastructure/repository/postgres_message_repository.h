#pragma once

#include "message_repository.h"
#include <pqxx/pqxx>
#include <string>

namespace chatserver::infrastructure::repository {

class PostgresMessageRepository final : public MessageRepository {
public:
    explicit PostgresMessageRepository(const std::string& connStr);

    std::int64_t save(const chatserver::domain::message::Message& message) override;

private:
    std::string connStr_;
};

}

