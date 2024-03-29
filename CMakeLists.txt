cmake_minimum_required(VERSION 3.10)

# set the project name
project(myserver)

add_executable(myserver myServer.cpp main.cpp)
target_include_directories(myserver PRIVATE .)

# target_link_libraries(Http libwsock32.a)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)