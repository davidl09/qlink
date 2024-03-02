add_executable(
        "${PROJECT_NAME}"
        main.cpp
        database.cpp
        server.cpp
)

target_precompile_headers(
        "${PROJECT_NAME}"
        PUBLIC
        "defs.h"
)

target_include_directories(
        "${PROJECT_NAME}"
        PUBLIC
        "${CROW_INCLUDE_DIR}"
        "${INJA_INCLUDE_DIR}"
        "${MAIL_INCLUDE_DIR}"
)

target_link_libraries(
        "${PROJECT_NAME}"
        PUBLIC
        crypto ssl
)