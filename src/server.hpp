//
// Created by davidl09 on 2/18/24.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "defs.h"
#include "database.hpp"
#include "curlpp/Options.hpp"


class webServer {
public:
    webServer();
    ~webServer();

    void run(int port, const std::string& dbFile = "db.sql", LogLevel logging = LogLevel::Debug);
    void run_async(int port, LogLevel logging = LogLevel::Debug);
private:
    App<CookieParser, Session> app;
    Database db;

    static std::string sha256Hash(std::string_view str);
};



#endif //SERVER_HPP
