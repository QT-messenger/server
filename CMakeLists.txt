cmake_minimum_required(VERSION 3.15)

project(ms-server LANGUAGES CXX)

set(SERVER_THREADS_COUNT "1" CACHE STRING "Count of threads that the server prefers to use.")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(include/)

find_package(Boost REQUIRED COMPONENTS json)
find_package(OpenSSL REQUIRED)

include_directories(src/)

add_executable(${PROJECT_NAME}
    src/main.cxx
    src/http_session.cxx
    src/listener.cxx
    src/websocket_session.cxx
    src/websocket_handlers.cxx
)
target_compile_definitions(${PROJECT_NAME} PUBLIC THREADS_COUNT=${SERVER_THREADS_COUNT})

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::json)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)