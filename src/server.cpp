//
// Created by davidl09 on 2/18/24.
//

#include "server.hpp"

#include <openssl/sha.h>
#include <openssl/md5.h>
#include <string>

webServer::webServer()
    : db("database.db3", "schema.sql"){

    CROW_ROUTE(app, "/")([](response& response) {
        response.set_static_file_info("static/new.html");
        response.end();
    });

    CROW_ROUTE(app, "/<string>").methods(HTTPMethod::GET)([this](const request& request, response& response, const std::string& urlHash) {
        auto serveStatic = [this](std::string str, crow::response& response) {
            auto maybeStaticFile = "./static/" + str;
            if (fs::exists(maybeStaticFile)) {
                response.set_static_file_info(maybeStaticFile);
                response.code = OK;
                response.end();
            }
        };

        serveStatic(urlHash, response);

        if (urlCache.contains(urlHash)) {
            response.redirect(urlCache[urlHash]);
            response.end();
        }
        else {
            try {
                const auto url = db.getURL(urlHash);
                urlCache[urlHash] = url;
                response.redirect("/" + url);
                response.end();
            } catch (std::exception& e) {
                response.code = NOT_FOUND;
                response.end("The requested URL does not exist");
            }
        }

    });

    CROW_ROUTE(app, "/createURL").methods(HTTPMethod::POST)([this](const request& request, response& response) {
        auto data = nlo::json::parse(request.body);
        if (!data.contains("url")) { //check request
            response.code = BAD_REQUEST;
            response.end("There was data missing from your request");
        }

        const auto url = data.at("url");
        const auto hashedURL = hashURL(url);

        if (!(db.urlExists(hashedURL))) { //if the requested url is not already registered
            db.addURL(url, hashedURL);
            urlCache[hashedURL] = url; //cache url
        }

        response.code = OK;
        response.write(to_string(nlo::json({"url", "/" + hashedURL})));
        response.end();
    });


}

std::string webServer::sha256Hash(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
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

std::string webServer::readFileToString(const std::string& filename) {
    std::stringstream ss;
    std::fstream stream(filename);
    if (!stream) {
        throw std::invalid_argument("Could not open file named " + filename);
    }
    ss << stream.rdbuf();
    return ss.str();
}

std::string webServer::hashURL(const std::string& longUrl) {
    std::vector<unsigned char> buffer(16), urlBuf(longUrl.size());
    ranges::copy(longUrl.begin(), longUrl.end(), urlBuf.begin());
    MD5(&urlBuf[0], urlBuf.size(), &buffer[0]);

    for (auto i = buffer.begin(), j = buffer.begin() + 8; i != buffer.begin() + 8 && j != buffer.end(); ++i, ++j) {
        *i ^= *j;
    }
    buffer.resize(8);

    auto constraint = [](unsigned char& c) {
        c %= 62;
        if (c <= 9) {
            c += '0';
        } else if (c <= 35) {
            c += 'A' - 10;
        } else if (c <= 61) {
            c += 'a' - 36;
        }
    };

    ranges::for_each(buffer, constraint);
    return {buffer.begin(), buffer.end()};
}

void webServer::run(const int port, LogLevel logging) {
    app.port(port).multithreaded().loglevel(logging).run();
}

webServer::~webServer() {
    for (const auto& urlPair : urlCache) {
        if (!db.urlExists(urlPair.first)) {
            db.addURL(urlPair.second, urlPair.first);
        }
    }
}

