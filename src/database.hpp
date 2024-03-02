//
// Created by davidl09 on 2/17/24.
//

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "defs.h"
#include "ddl_defs.h"




class Database {
public:
    Database(const std::string& dbFile);

    bool addUser(const nlo::json& userData);
    bool checkLogin(const std::string& email, const std::string& passHash);

private:
    sqlpp::sqlite3::connection_config config;
    sqlpp::sqlite3::connection db;
};






#endif //DATABASE_HPP
