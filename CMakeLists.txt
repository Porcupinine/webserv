cmake_minimum_required(VERSION 3.23)
project(webserv)

set(CMAKE_CXX_STANDARD 11)

set(SRC
		src/cgiHandler.cpp
		src/CGI.h
		src/response.cpp
		includes/response.h
		src/Cgi.cpp
		includes/Cgi.h)

set(TESTSRC )

add_executable(webserv ${SRC})