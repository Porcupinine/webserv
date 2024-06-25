#include <iostream>
#include "webSocket.h"
#include "defines.h"
#include <poll.h>
#include "../Request/parseRequest.hpp"
#include "../includes/cgiHandler.h"
#include <unistd.h>

int main() {
	webserv::webSocket serv = webserv::webSocket(127, SERV_PORT);
	serv.createSocket();
	serv.read();
//	nfds_t test;
//	std::string message ("POST /upload HTTP/1.1\r\n"
//						 "Host: example.com\r\n"
//						 "Content-Type: multipart/form-data;boundary=----WebKitFormBoundaryE19zNvXGzXaLvS5C\r\n"
//						 "Content-Length: 123456\r\n"
//						 "\r\n"
//						 "------WebKitFormBoundaryE19zNvXGzXaLvS5C\r\n"
//						 "Content-Disposition: form-data; name=\"file\";filename=\"example.jpg\"\r\n"
//						 "Content-Type: image/jpeg\r\n\r\n"
//						 "\xFF\x1C\x1C\x20\x24\x2E\x27\x20\x22\x2C\x23\x1C\x1C\x28\x37\x29\x1C\30\x31\x34\x34\x34\x1F\x27\x39\x3D\x38\x32\x3C\x2E\x33\x34\x32\r\n"
//						 "------WebKitFormBoundaryE19zNvXGzXaLvS5C--\r\n");
//	std::string message ("DELETE /users/123 HTTP/1.1\r\n"
//						 "Host: example.com\r\n"
//						 "Content-Type: application/json\r\n"
//						 "Content-Length: 26\r\n"
//						 "\r\n"
//						 "{"
//						 "\"reason\": \"User request\""
//						 "}");
//std::string message ("POST /users/123 HTTP/1.1\r\n"
//					 "Host: example.com\r\n"
//					 "Content-Type: application/json\r\n"
//					 "Content-Length: 67\r\n"
//					 "\r\n"
//					 "{"
//					 "\"name\": \"John Doe\","
//					 "\"email\": \"john.doe@example.com\","
//					 "\"status\": \"active\""
//					 "}");
//	std::cout<<"parsing request \n";
//	parseRequest request(message);
//	std::cout << "version: " << request.getVersion() << '\n';
//	std::cout << "ret value: " << request.getRetVal() << '\n';
//	std::cout << "body: " << request.getBodyMsg() << '\n';
//	std::cout << "method: " << request.getMethod() << '\n';
//	std::cout << "path: " << request.getPath() << '\n';
//	std::cout << "port: " << request.getPort() << "\n\n";
//	std::cout<< "calling cgi\n";
//	cgiHandler(request);
//	char *argv[] = {"./../cgi-bin/test.py", nullptr}; //path and NULL
//	if (execve(argv[0], argv, nullptr) == -1) {
//		std::cout<<"This is no middle age\n";
//	}
	return 0;
}

