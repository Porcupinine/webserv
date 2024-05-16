cmake_minimum_required(VERSION 3.23)
project(webserv)

set(CMAKE_CXX_STANDARD 11)

set(SRC
		src/CGI.cpp
		src/CGI.h)

set(TESTSRC )

add_executable(webserv ${SRC})