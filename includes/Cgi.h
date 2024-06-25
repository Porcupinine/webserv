////
//// Created by laura on 27-5-24.
////
//
//#ifndef WEBSERV_CGI_H
//#define WEBSERV_CGI_H
//
//#include <map>
//#include <string>
//
//class Cgi {
//public:
//	std::map<unsigned int, std::string> m_errorCodes;
//
//	m_errorCodes[400] = "HTTP/1.1 400 Bad Request\r\n\n"
//	"Content-Type: text/html\r\n\nContent-Length: 151\r\n\r\n "
//	"<!DOCTYPE html><html><head><title>400</title></head><body><h1> 400 Bad Request Error! </h1><p>We are not speaking the same language!</p></body></html>";
//	m_errorCodes[403] = "HTTP/1.1 403 Forbiden\r\n\n"
//	"Content-Type: text/html\r\n\nContent-Length: 130\r\n\r\n "
//	"<!DOCTYPE html><html><head><title>403</title></head><body><h1> 403 Forbiden! </h1><p>This is top secret, sorry!</p></body></html>";
//	m_errorCodes[404] = "HTTP/1.1 404 Not Found\r\n\n"
//	"Content-Type: text/html\r\n\nContent-Length: 115\r\n\r\n "
//	"<!DOCTYPE html><html><head><title>404</title></head><body><h1> 404 Page not found! </h1><p>Puff!</p></body></html>";
//	m_errorCodes[405] = "HTTP/1.1 405 Method Not Allowed\r\n\n"
//	"Content-Type: text/html\r\n\nContent-Length: 139\r\n\r\n "
//	"<!DOCTYPE html><html><head><title>405</title></head><body><h1> 405 Method Not Allowed! </h1><p>We forgot how to do that!</p></body></html>";
//	m_errorCodes[413] = "HTTP/1.1 413 Payload Too Large\r\n\n"
//	"Content-Type: text/html\r\n\nContent-Length: 163\r\n\r\n "
//	"<!DOCTYPE html><html><head><title>413</title></head><body><h1> 413 Payload Too Large! </h1><p>We are too busy right now, please try again later!</p></body></html>";
//	m_errorCodes[500] = "HTTP/1.1 500 Internal Server Error\r\n\n"
//	"Content-Type: text/html\r\n\nContent-Length: 146\r\n\r\n "
//	"<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
//};
//
//
//
////std::map<unsigned int, std::string> m_errorCodes;
////
////m_errorCodes.clean();
//
//
//#endif //WEBSERV_CGI_H
