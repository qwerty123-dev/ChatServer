#pragma once

#include <string>

#include "chatserver/domain/services/message_encryptor.h"

namespace chatserver::infrastructure::crypto {

class OpenSSLMessageEncryptor final
    : public domain::services::MessageEncryptor {
public:
    explicit OpenSSLMessageEncryptor(const std::string& secret);

    std::string encrypt(const std::string& plaintext) const override;
    std::string decrypt(const std::string& ciphertext) const override;

private:
    std::string secret_;
};

}

