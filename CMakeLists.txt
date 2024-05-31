cmake_minimum_required(VERSION 3.27)
project(webserv)

set(CMAKE_CXX_STANDARD 17)

set(SRC
		src/cgiHandler.cpp
		src/response.cpp
		includes/response.h
		src/Cgi.cpp
		includes/Cgi.h
		Request/parseRequest.cpp
		Request/utilsRequest.cpp)

set(SRCTEST
		test/main.cpp
		test/webSocket.cpp
		test/webSocket.h
		test/defines.h
		test/webPoll.cpp
		test/webPoll.h)

set(TESTSRC )

add_executable(webserv ${SRC} ${SRCTEST})