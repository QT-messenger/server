cmake_minimum_required(VERSION 3.15)

project(ms-server LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(include/)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(src/)

message(STATUS ${Boost_FOUND})

add_executable(ms-server
    src/main.cxx
    src/http_session.cxx
    src/listener.cxx
    src/websocket_session.cxx
    
)

target_link_libraries(ms-server PRIVATE ${Boost_LIBRARIES})
target_link_libraries(ms-server PRIVATE OpenSSL::SSL)