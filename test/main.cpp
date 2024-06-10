#include <iostream>
#include "webSocket.h"
#include "defines.h"
#include <poll.h>
#include "../Request/parseRequest.hpp"
#include "../includes/cgiHandler.h"

int main() {
//	webserv::webSocket serv = webserv::webSocket(127, SERV_PORT);
//	serv.createSocket();
//	serv.read();
//	nfds_t test;
//	std::string message ("POST /upload HTTP/1.1\r\n"
//						 "Host: example.com\r\n"
//						 "Content-Type: multipart/form-data;boundary=----WebKitFormBoundaryE19zNvXGzXaLvS5C\r\n"
//						 "Content-Length: 123456\r\n\r\n"
//						 "------WebKitFormBoundaryE19zNvXGzXaLvS5C\r\n"
//						 "Content-Disposition: form-data; name=\"file\";filename=\"example.jpg\"\r\n"
//						 "Content-Type: image/jpeg\r\n\r\n"
//						 "(binary data of the image file goes here)\r\n"
//						 "------WebKitFormBoundaryE19zNvXGzXaLvS5C--\r\n");
	std::string message ("DELETE /users/123 HTTP/1.1\r\n"
						 "Host: example.com\r\n"
						 "Content-Type: application/json\r\n"
						 "Content-Length: 26\r\n"
						 "\r\n"
						 "{"
						 "\"reason\": \"User request\""
						 "}");
	std::cout<<"parsing request \n";
	parseRequest request(message);
	std::cout << "version: " << request.getVersion() << '\n';
	std::cout << "ret value: " << request.getRetVal() << '\n';
	std::cout << "body: " << request.getBodyMsg() << '\n';
	std::cout << "method: " << request.getMethod() << '\n';
	std::cout << "path: " << request.getPath() << '\n';
	std::cout << "port: " << request.getPort() << "\n\n";
	std::cout<< "calling cgi\n";
	cgiHandler(request);
	return 0;
}

