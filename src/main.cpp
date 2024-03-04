#include "defs.h"
#include "server.hpp"

int main() {
    webServer server;
    server.run(8080, LogLevel::INFO);
}
