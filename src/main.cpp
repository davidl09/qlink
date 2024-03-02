#include <iostream>
#include "defs.h"
#include "server.hpp"
#include "database.hpp"

int main() {
    Database db("database.db3");
    //db.addUser({{"firstName", "David"}, {"lastName", "Laeer"}, {"email", "example@example.com"}, {"passHash", ";alsdkfj;aslkfjs;a"}});
    std::cout << db.checkLogin("example@example.com", ";alsdkfj;aslkfjs;a");
}
