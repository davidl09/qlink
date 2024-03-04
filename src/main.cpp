#include "defs.h"
#include "server.hpp"

int main(int argc, char *argv[]) {
    int port = 8080;
    if (argc > 1) {
        errno = 0;
        char *endptr = nullptr;
        port = strtol(argv[1], &endptr, 10);
        if (errno || !endptr) {
            std::cout << "Invalid port setting, defaulting to " << 8080 << '\n';
            port = 8080;
        }
        else std::cout << "Port set to " << port << '\n';
    }
    else std::cout << "No port specified, defaulting to 8080\n";
    webServer server;
    server.run(port, LogLevel::Info);
}
