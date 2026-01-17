#include <gtest/gtest.h>
#include "chatserver/infrastructure/crypto/openssl_password_hasher.h"
#include "chatserver/domain/user/password_hash.h"

using namespace chatserver::infrastructure::crypto;
using namespace chatserver::domain;

TEST(PasswordHasher, HashAndVerify) {
    OpenSSLPasswordHasher hasher;

    std::string plain = "secret123";
    PasswordHash h = hasher.hash(plain);

    // Хеш должен быть непустым и содержать разделитель salt:hash
    std::string s = h.value();
    ASSERT_FALSE(s.empty());
    ASSERT_NE(s.find(':'), std::string::npos);

    // verify должен вернуть true для правильного пароля
    EXPECT_TRUE(hasher.verify(plain, h));

    // verify должен вернуть false для неверного пароля
    EXPECT_FALSE(hasher.verify("wrongpassword", h));
}

TEST(PasswordHasher, InvalidFormatVerify) {
    OpenSSLPasswordHasher hasher;
    // Неправильный формат хеша
    PasswordHash bad("not-a-valid-hash");
    EXPECT_FALSE(hasher.verify("anything", bad));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

