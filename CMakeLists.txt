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
    src/Webserv/WebServ.cpp
    src/Server/Server.cpp
    src/Config/Config.cpp
    src/VirtualHost/VirtualHost.cpp
	src/Utils/FdManagement.cpp
	src/Utils/StructUtils.cpp
	src/CGI/CgiHandler.cpp
	src/Request/ParseRequest.cpp
	src/Request/UtilsRequest.cpp
	src/Response/Header.cpp 
	src/Response/Response.cpp 
	src/Response/Utils.cpp
	src/Cookies/Cookies.cpp
)


include_directories(inc)

