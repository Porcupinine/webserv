cmake_minimum_required(VERSION 3.22)
project(webserver)

set(CMAKE_CXX_STANDARD 17)

# add_compile_options(-fstandalone-debug)
# add_compile_options(-Wall -Wextra -Werror -g)
#add_compile_options(-fsanitize=address -fsanitize=leak -g3)
#add_link_options(-fsanitize=address -fsanitize=leak)

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
	src/utils/fdManagement.cpp
	src/utils/structUtils.cpp
	# src/utils.cpp
	src/cgiHandler.cpp
	src/Cgi.cpp
	Request/parseRequest.cpp
	Request/utilsRequest.cpp
	Response/header.cpp 
	Response/Response.cpp 
	Response/utils.cpp
	Cookies/Cookies.cpp
)


include_directories(inc)

