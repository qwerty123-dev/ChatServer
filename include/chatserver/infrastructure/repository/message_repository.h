#pragma once

#include <memory>
#include "chatserver/domain/message/message.h"

namespace chatserver::infrastructure::repository {

class MessageRepository {
public:
    virtual ~MessageRepository() = default;
    virtual std::int64_t save(const chatserver::domain::message::Message& message) = 0;
};

}

