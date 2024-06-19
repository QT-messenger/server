cmake_minimum_required(VERSION 3.15)

project(ms-server LANGUAGES CXX)

set(SERVER_THREADS_COUNT "1" CACHE STRING "Count of threads that the server prefers to use.")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(include/)

find_package(Boost REQUIRED COMPONENTS json)
find_package(OpenSSL REQUIRED)
find_package(libpqxx REQUIRED)

include_directories(src/)

add_executable(${PROJECT_NAME}
    src/main.cxx
    src/http_session.cxx
    src/listener.cxx
    src/websocket_session.cxx
    src/websocket_handlers.cxx
)

if("X$ENV{SERVER_DB_USER}" STREQUAL "X")
    message(FATAL_ERROR "Not defined database user, set environment variable SERVER_DB_USER")
endif()

if("X$ENV{SERVER_DB_PASSWORD}" STREQUAL "X")
    message(FATAL_ERROR "Not defined database password, set environment variable SERVER_DB_PASSWORD")
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC THREADS_COUNT=${SERVER_THREADS_COUNT} DB_USER=$ENV{SERVER_DB_USER} DB_PASSWORD=$ENV{SERVER_DB_PASSWORD})

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::json)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)
target_include_directories(${PROJECT_NAME} PRIVATE libpqxx::pqxx)
target_link_libraries(${PROJECT_NAME} PRIVATE libpqxx::pqxx)