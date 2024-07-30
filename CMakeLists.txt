cmake_minimum_required(VERSION 3.22)
project(webserver)

set(CMAKE_CXX_STANDARD 17)

set(SRCTEST
		test/main.cpp
		test/webSocket.cpp
		test/webSocket.h
		test/defines.h
		test/webPoll.cpp
		test/webPoll.h)

add_executable(webserver
    src/main.cpp
    src/webserv/WebServ.cpp
    src/server/Server.cpp
    src/config/Config.cpp
    src/virtualHost/VirtualHost.cpp
    src/utils.cpp
	src/cgiHandler.cpp
	src/response.cpp
	includes/response.h
	src/Cgi.cpp
	includes/Cgi.h
	Request/parseRequest.cpp
	Request/utilsRequest.cpp
)


include_directories(inc)

