//
// Created by davidl09 on 2/17/24.
//

#ifndef DEFS_H
#define DEFS_H

#include "crow.h"
#include "crow/middleware_context.h"
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/session.h"
#include "inja.hpp"
#include "curlpp/cURLpp.hpp"
#include "SMTPClient.h"
#include "sqlpp11/sqlpp11.h"
#include "sqlpp11/column_types.h"
#include "sqlpp11/sqlite3/sqlite3.h"

using namespace crow;
using Session = SessionMiddleware<FileStore>;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace chrono = std::chrono;
namespace nlo = nlohmann;

#define SQL_QSTR(x) #x

#endif //DEFS_H
