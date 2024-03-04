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

    static std::string readFileToString(const std::string& filename);

    static constexpr auto MAX_REQ_PER_HOUR = 60u;

private:
    struct IpFreqGuard : ILocalMiddleware {
        struct context {
            std::unordered_map<uint32_t, std::deque<chrono::time_point<chrono::system_clock, chrono::seconds>>> ipAddresses;
        };
        uint32_t ipFromStr(const std::string& ip);
        void before_handle(const request& request, response& response, context& context, detail::context<IpFreqGuard>&);
        void after_handle(const request&, response&, context&, detail::context<IpFreqGuard>&) {}
    };


    App<IpFreqGuard> app;
    Database db;
    std::unordered_map<std::string, std::string> urlCache;

    static std::string sha256Hash(const std::string& str);
    static std::string hashURL(const std::string& longUrl);
};



#endif //SERVER_HPP
