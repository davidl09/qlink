//
// Created by davidl09 on 2/17/24.
//

#include "database.hpp"

Database::Database(const std::string& dbFile)
    :   config(dbFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE),
        db(config) {}

bool Database::addUser(const nlo::json& userData) {
    db::Users users_;
    try {
        db(insert_into(users_).set(
            users_.email = std::string{userData.at("email")},
            users_.passHash = std::string{userData.at("passHash")},
            users_.firstName = std::string{userData.at("firstName")},
            users_.lastName = std::string{userData.at("lastName")},
            users_.misc = std::string{(userData.contains("misc") ? userData.at("misc") : "")}
        ));
        return true;
    }
    catch (std::exception& e) {
        std::clog << e.what();
        return false;
    }
}

bool Database::checkLogin(const std::string& email, const std::string& passHash) {
    db::Users users;
    auto results = db(select(users.email, users.passHash).from(users).where(users.email == email and users.passHash == passHash));
    return !results.empty();
}

