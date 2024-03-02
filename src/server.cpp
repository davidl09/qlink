//
// Created by davidl09 on 2/18/24.
//

#include "server.hpp"

#include <openssl/sha.h>
#include <string>

webServer::webServer()
    : app(Session{FileStore{"./tmp/sessiondata"}}) {

    CROW_CATCHALL_ROUTE(app)([]() {
        std::stringstream ss;
        ss << std::chrono::steady_clock::now().time_since_epoch();
        return ss.str();
    });
}

std::string webServer::sha256Hash(std::string_view str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.begin(), str.size());
    SHA256_Final(hash, &sha256);

    std::string hashedString;

    std::for_each(hash, hash + SHA256_DIGEST_LENGTH, [&hashedString](const auto& byte) {
        char hex[3] = {0};
        sprintf(hex, "%02x", byte);
        hex[2] = 0;
        hashedString.append(hex);
    });

    return hashedString;
}

void webServer::run(const int port,  const std::string& dbFile , LogLevel logging) {
    app.port(port).multithreaded().run();
}

webServer::~webServer() = default;

