cmake_minimum_required(VERSION 3.10)

# Set the project name I keep forgetting this.
project(WebServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(webserver
    src/main.cpp
    src/webserv/WebServ.cpp
    src/server/Server.cpp
    src/config/Config.cpp
    src/utils.cpp
)

include_directories(inc)
