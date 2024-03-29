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
        response.set_static_file_info(CROW_STATIC_DIRECTORY"new.html");
        response.end();
    });

    CROW_ROUTE(app, "/createURL")
    .methods(HTTPMethod::POST)
    .CROW_MIDDLEWARES(app, IpFreqGuard)
    ([this](const request& request, response& response){
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

    CROW_ROUTE(app, "/createCustom")
    .methods(HTTPMethod::Post)
    .CROW_MIDDLEWARES(app, IpFreqGuard)
    ([this](const request& request, response& response){
        auto data = nlo::json::parse(request.body);
        auto body = nlo::json::object();
        body["error"] = "";

        if (not data.contains("url") or not data.contains("customUrl")) {
            response.code = BAD_REQUEST;
            body["error"] = "Missing parameters in request";
            response.end(to_string(body));
            return;
        }

        const std::string url{data.at("url")}, customUrl{data.at("customUrl")};

        if (customUrl.length() > 8) {
            response.code = OK;
            body["error"] = "Custom URL length must not exceed 8 characters";
            response.end(to_string(body));
            return;
        }

        if (db.urlExists(customUrl)) {
            response.code = OK;
            body["error"] = "The requested URL already exists";
            response.end(to_string(body));
            return;
        }

        try {
            if (!db.addURL(url, customUrl)) {
                response.code = OK;
                body["error"] = "The requested url already exists";
                response.end(to_string(body));
                return;
            }
        }
        catch (...) {
            response.code = INTERNAL_SERVER_ERROR;
            body["error"] = "There was an error adding your url, please contact support";
            response.end(to_string(body));
            return;
        }

        response.code = OK;

        response.end(to_string(data));
    });

    CROW_ROUTE(app, "/isavail")
    .methods(HTTPMethod::Post)
    ([this](const request& request, response& response) {
        nlo::json data;

        auto respondBadReq = [this, &response](const std::string& message) {
            response.code = BAD_REQUEST;
            response.end(message);
        };

        try {
            data = nlo::json::parse(request.body);
            if (not data.contains("customUrl")) throw;
        }
        catch (nlo::json::parse_error& e) {
            respondBadReq(e.what());
        }

        response.code = OK;
        const auto status = not db.urlExists(data.at("customUrl"));
        response.end(to_string(nlo::json::object({{"result", status}})));
    });

    CROW_ROUTE(app, "/q/<string>").methods(HTTPMethod::GET)([this](const request& request, response& response, const std::string& urlHash) {
        const auto maybeStaticFile = CROW_STATIC_DIRECTORY + urlHash;
        if (fs::exists(maybeStaticFile)) {
            response.set_static_file_info(maybeStaticFile);
            response.code = OK;
            response.end();
            return;
        }

        if (urlCache.contains(urlHash)) {
            response.redirect(urlCache[urlHash]);
            response.end();
            return;
        }
        try {
            const auto url = db.getURL(urlHash);
            urlCache[urlHash] = url;
            response.moved("https://qli.ink/q/" + url);
            response.end();
        } catch (std::exception& e) {
            response.code = NOT_FOUND;
            response.end("The requested URL does not exist");
        }


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
    app.port(port).multithreaded()
#ifdef CROW_ENABLE_COMPRESSION
    .use_compression(compression::algorithm::GZIP)
#endif
    .loglevel(logging).run();
}

webServer::~webServer() {
    for (const auto& urlPair : urlCache) {
        if (!db.urlExists(urlPair.first)) {
            db.addURL(urlPair.second, urlPair.first);
        }
    }
}

void webServer::IpFreqGuard::before_handle(const request& request, response& response, context& context, detail::context<IpFreqGuard>&) {
    const auto ip = ipFromStr(request.remote_ip_address);
    const auto now = chrono::time_point_cast<chrono::seconds>(chrono::system_clock::now());
    context.ipAddresses[ip].push_front(now);

    for (auto it = context.ipAddresses[ip].rbegin(); it != context.ipAddresses[ip].rend() && now - *it > 1h; ++it) {
        context.ipAddresses[ip].pop_back();
    }

    if (context.ipAddresses[ip].size() > MAX_REQ_PER_HOUR) {
        response.code = FORBIDDEN;
        response.end();
    }
}

uint32_t webServer::IpFreqGuard::ipFromStr(const std::string& ip) {
    std::vector<uint32_t> parts;
    std::stringstream ss{ip};
    std::string part;

    while(std::getline(ss, part, '.')) {
        parts.push_back(std::stoi(part));
    }

    if (parts.size() != 4) return 0;

    uint32_t res = 0;
    for (auto i = 0; i < parts.size(); ++i) {
        if (parts[i] > 255) {
            return 0;
        }
        res |= (parts[i] << (8 * (3 - i)));
    }
    return res;
}



