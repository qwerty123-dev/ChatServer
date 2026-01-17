#include "chatserver/domain/message/message_text.h"

#include <stdexcept>

namespace chatserver::domain {

MessageText::MessageText(std::string value)
    : value_(std::move(value)) {
    if (value_.empty()) {
        throw std::invalid_argument("Message text cannot be empty");
    }
}

const std::string& MessageText::value() const {
    return value_;
}

}

