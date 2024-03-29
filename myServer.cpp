#include "myServer.hpp"

#include <sstream>
#include <string.h>
#include <unistd.h>

#define BACKLOG     1
#define BUFFER_SIZE 30000

MyServer::MyServer(std::string ipAddress, std::string port) : _ipAddress(ipAddress), _port(port), _listenSocket(),
    _newSocket(), _incomingMsg(), _serverMsg(buildResponse()), _hints(), _res(nullptr), _result(nullptr)
{
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(_ipAddress.c_str(), _port.c_str(), &_hints, &_result) != 0){
        std::cerr << "Couldn't get AddressInfo\n";
        exit (1);
    }
    _res = _result;

    if (startServer() != 0){
        std::cerr << "For some reason we couldn't start the server\n";
    }
}

MyServer::~MyServer() {
    closeServer();
}

int MyServer::startServer() {
    _listenSocket = socket(_res->ai_family, _res->ai_socktype, _res->ai_protocol);
    if (_listenSocket < 0){
        perror("socket creation");
        exit(1);
    }

    if (bind(_listenSocket, _res->ai_addr, _res->ai_addrlen) < 0){
        perror("Couldn't bind");
        close(_listenSocket);
        exit(1);
    }
    freeaddrinfo(_res);
    return (0);
}

void MyServer::startListening() {
    if (listen(_listenSocket, BACKLOG) < 0){
        perror("My ears are broken.");
        exit(1);
    }

    int bytesRead;
    while (1){
        std::cout << "========= Waiting for a new connection =========\n\n";
        acceptConnection(_newSocket);

        char buffer[BUFFER_SIZE] = {0};
        bytesRead = read(_newSocket, buffer, BUFFER_SIZE);
        if (bytesRead < 0){
            std::cerr << "Couldn't read bytes from client connection\n";
        }

        std::cout << "------ RECEIVED REQUEST ------\n\n";

        sendResponse();
        close(_newSocket);
    }
}

void MyServer::acceptConnection(int &newSocket) {
    struct sockaddr_storage newSocketStuff;
    socklen_t newSocketStuffSize = sizeof(newSocketStuff);

    newSocket = accept(_listenSocket, (struct sockaddr *)&newSocketStuff, &newSocketStuffSize);
    if (newSocket < 0){
        std::cout << "Couldn't accept incoming connection\n";
        exit(1);
    }
}

std::string MyServer::buildResponse()
{
    std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> NOW WE KNOW :) </p></body></html>";
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
       << htmlFile;

    return ss.str();
}

void MyServer::sendResponse()
{
    long bytesSent;

    bytesSent = write(_newSocket, _serverMsg.c_str(), _serverMsg.size());
    if (bytesSent == _serverMsg.size()){
        std::cout << "------ Server Response sent to client ------\n\n" << std::endl;
    }
    else{
        std::cout << "Error sending response to client"<< std::endl;
    }
}

void MyServer::closeServer() {
    close(_listenSocket);
    close(_newSocket);
    exit(0);
}