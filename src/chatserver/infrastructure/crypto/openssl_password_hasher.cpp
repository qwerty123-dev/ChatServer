#include "chatserver/infrastructure/crypto/openssl_password_hasher.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstring>

namespace chatserver::infrastructure::crypto {

static std::string to_hex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i)
        oss << std::setw(2) << static_cast<int>(data[i]);
    return oss.str();
}

static std::vector<unsigned char> from_hex(const std::string& hex) {
    if (hex.size() % 2 != 0) throw std::runtime_error("invalid hex length");
    std::vector<unsigned char> out(hex.size() / 2);
    for (size_t i = 0; i < out.size(); ++i) {
        auto byte = hex.substr(i * 2, 2);
        out[i] = static_cast<unsigned char>(std::stoi(byte, nullptr, 16));
    }
    return out;
}

static std::string openssl_last_error() {
    unsigned long err = ERR_get_error();
    if (err == 0) return std::string();
    char buf[256];
    ERR_error_string_n(err, buf, sizeof(buf));
    return std::string(buf);
}

domain::PasswordHash OpenSSLPasswordHasher::hash(const std::string& password) const {
    try {
        // Генерируем соль
        std::vector<unsigned char> salt(SALT_LEN);
        if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1) {
            std::string err = openssl_last_error();
            std::cerr << "[OpenSSLPasswordHasher] RAND_bytes failed: " << err << std::endl;
            throw std::runtime_error("RAND_bytes failed");
        }

        // Вычисляем PBKDF2-HMAC-SHA256
        std::vector<unsigned char> out(HASH_LEN);
        if (PKCS5_PBKDF2_HMAC(
                password.c_str(),
                static_cast<int>(password.size()),
                salt.data(),
                static_cast<int>(salt.size()),
                ITERATIONS,
                EVP_sha256(),
                HASH_LEN,
                out.data()
            ) != 1) {
            std::string err = openssl_last_error();
            std::cerr << "[OpenSSLPasswordHasher] PBKDF2 failed: " << err << std::endl;
            throw std::runtime_error("PBKDF2 failed");
        }

        // Формируем строку salt:hash в hex
        std::string combined = to_hex(salt.data(), salt.size()) + ":" + to_hex(out.data(), out.size());
        return domain::PasswordHash(combined);
    }
    catch (const std::exception& ex) {
        std::cerr << "[OpenSSLPasswordHasher::hash] exception: " << ex.what() << std::endl;
        throw;
    }
}

bool OpenSSLPasswordHasher::verify(const std::string& password,
                                   const domain::PasswordHash& storedHash) const {
    try {
        auto stored = storedHash.value();
        auto pos = stored.find(':');
        if (pos == std::string::npos) {
            std::cerr << "[OpenSSLPasswordHasher::verify] invalid stored hash format" << std::endl;
            return false;
        }

        auto salt_hex = stored.substr(0, pos);
        auto hash_hex = stored.substr(pos + 1);

        auto salt = from_hex(salt_hex);
        auto expected = from_hex(hash_hex);

        if (salt.size() != SALT_LEN || expected.size() != HASH_LEN) {
            std::cerr << "[OpenSSLPasswordHasher::verify] unexpected lengths" << std::endl;
            return false;
        }

        std::vector<unsigned char> out(HASH_LEN);
        if (PKCS5_PBKDF2_HMAC(
                password.c_str(),
                static_cast<int>(password.size()),
                salt.data(),
                static_cast<int>(salt.size()),
                ITERATIONS,
                EVP_sha256(),
                HASH_LEN,
                out.data()
            ) != 1) {
            std::string err = openssl_last_error();
            std::cerr << "[OpenSSLPasswordHasher::verify] PBKDF2 failed: " << err << std::endl;
            return false;
        }

        // безопасное сравнение
        return CRYPTO_memcmp(out.data(), expected.data(), HASH_LEN) == 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "[OpenSSLPasswordHasher::verify] exception: " << ex.what() << std::endl;
        return false;
    }
}

}

