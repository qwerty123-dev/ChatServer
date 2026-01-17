#include "chatserver/infrastructure/crypto/openssl_message_encryptor.h"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <vector>
#include <sstream>
#include <iomanip>

namespace chatserver::infrastructure::crypto {

static std::string to_hex(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
    return oss.str();
}

static std::vector<unsigned char> from_hex(const std::string& hex) {
    std::vector<unsigned char> out(hex.size() / 2);
    for (size_t i = 0; i < out.size(); ++i)
        out[i] = static_cast<unsigned char>(
            std::stoi(hex.substr(i * 2, 2), nullptr, 16)
        );
    return out;
}

OpenSSLMessageEncryptor::OpenSSLMessageEncryptor(
    const std::string& secret
) : secret_(secret) {}

std::string OpenSSLMessageEncryptor::encrypt(
    const std::string& plaintext
) const {
    unsigned char key[32];
    EVP_Digest(secret_.data(), secret_.size(),
               key, nullptr, EVP_sha256(), nullptr);

    unsigned char iv[12];
    RAND_bytes(iv, sizeof(iv));

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, iv);

    std::vector<unsigned char> out(plaintext.size());
    int len;
    EVP_EncryptUpdate(
        ctx, out.data(), &len,
        reinterpret_cast<const unsigned char*>(plaintext.data()),
        plaintext.size()
    );

    int ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, out.data() + len, &len);
    ciphertext_len += len;

    unsigned char tag[16];
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);
    EVP_CIPHER_CTX_free(ctx);

    return to_hex(iv, sizeof(iv)) + ":" +
           to_hex(out.data(), ciphertext_len) + ":" +
           to_hex(tag, sizeof(tag));
}

std::string OpenSSLMessageEncryptor::decrypt(
    const std::string& encrypted
) const {
    auto p1 = encrypted.find(':');
    auto p2 = encrypted.find(':', p1 + 1);

    auto iv = from_hex(encrypted.substr(0, p1));
    auto data = from_hex(encrypted.substr(p1 + 1, p2 - p1 - 1));
    auto tag = from_hex(encrypted.substr(p2 + 1));

    unsigned char key[32];
    EVP_Digest(secret_.data(), secret_.size(),
               key, nullptr, EVP_sha256(), nullptr);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, iv.data());

    std::vector<unsigned char> out(data.size());
    int len;
    EVP_DecryptUpdate(ctx, out.data(), &len, data.data(), data.size());

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag.size(), tag.data());

    if (EVP_DecryptFinal_ex(ctx, out.data() + len, &len) <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);
    return std::string(out.begin(), out.end());
}

}

