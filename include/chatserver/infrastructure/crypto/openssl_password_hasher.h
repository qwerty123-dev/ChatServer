#pragma once

#include <string>
#include "chatserver/domain/user/password_hash.h"
#include "chatserver/domain/services/password_hasher.h"

namespace chatserver::infrastructure::crypto {

class OpenSSLPasswordHasher final : public domain::services::PasswordHasher {
public:
    domain::PasswordHash hash(const std::string& password) const override;
    bool verify(const std::string& password,
                const domain::PasswordHash& hash) const override;

private:
    static constexpr int ITERATIONS = 100000; 
    // 100k итераций - разумный минимум для PBKDF2-HMAC-SHA256
    static constexpr int SALT_LEN = 16;
    // 16 байт соли - стандарт
    static constexpr int HASH_LEN = 32;
    // 32 байта хэша - соответствует SHA-256
};

}

