//
// Created by davidl09 on 2/17/24.
//

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "defs.h"
#include "ddl_defs.h"




class Database {
public:
    Database(const std::string& dbFile, const std::string& configFile);

    bool addURL(const std::string& longURL, const std::string& shortURL);
    bool urlExists(const std::string& urlHash);
    std::string getURL(const std::string& shortURL);

private:
    sqlpp::sqlite3::connection_config config;
    sqlpp::sqlite3::connection db;
    std::mutex dbMutex;
};






#endif //DATABASE_HPP
