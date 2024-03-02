cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME crow-template)
project(${PROJECT_NAME} C CXX)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/server")

set(CROW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/crow/include")
set(INJA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inja/single_include/inja")
set(MAIL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mailclient-cpp/MAIL")


find_package(
        Sqlpp11
        COMPONENTS SQLite3
        REQUIRED
)


add_subdirectory(mailclient-cpp)
add_subdirectory(curlpp)
add_subdirectory(src)

target_link_libraries(
        "${PROJECT_NAME}"
        PUBLIC
        mailclient
        curlpp
        sqlpp11::sqlpp11
        sqlite3
        crypto
        ssl
        z
)
