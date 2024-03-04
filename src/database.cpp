//
// Created by davidl09 on 2/17/24.
//

#include "database.hpp"

Database::Database(const std::string& dbFile)
    :   config(dbFile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE),
        db(config) {}

std::string Database::getURL(const std::string& shortURL) {
    db::Urls urls;
    std::scoped_lock lock(dbMutex);
    auto result = db(select(all_of(urls)).from(urls).where(urls.hashStr == shortURL));
    if (result.empty()) throw std::invalid_argument("Requested URL not indexed");
    return result.begin()->url;
}

bool Database::urlExists(const std::string& urlHash) {
    db::Urls urls;
    auto result =  !db(select(all_of(urls)).from(urls).where(urls.hashStr == urlHash)).empty();
    return result;
}

bool Database::addURL(const std::string& longURL, const std::string& shortURL) {
    db::Urls urls;
    std::scoped_lock lock(dbMutex);
    auto result = db(select(all_of(urls)).from(urls).where(urls.hashStr == shortURL));
    for (const auto& r : result) {
        std::cout << r.hashStr << '\n';
    }
    if (!result.empty()) return false;

    try {
        db(insert_into(urls).set(urls.url = longURL, urls.hashStr = shortURL));
        return true;
    }
    catch(std::exception& e) {
        std::clog << e.what();
        return false;
    }

}



