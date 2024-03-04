//
// Created by davidl09 on 2/18/24.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "defs.h"
#include "database.hpp"


class webServer {
public:
    webServer();
    ~webServer();

    void run(int port, LogLevel logging = LogLevel::Debug);
    void run_async(int port, LogLevel logging = LogLevel::Debug);
private:
    crow::SimpleApp app;
    Database db;
    std::unordered_map<std::string, std::string> urlCache;

    static std::string sha256Hash(const std::string& str);
    static std::string readFileToString(const std::string& filename);
    static std::string hashURL(const std::string& longUrl);

};



#endif //SERVER_HPP
