#include "myServer.hpp"

#include <sstream>
#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <fcntl.h>

#define BACKLOG     1
#define MAX_EVENTS 5
#define BUFFER_SIZE 30000

MyServer::MyServer(std::string ipAddress, std::string port) : _ipAddress(ipAddress), _port(port), _listenSocket(),
    _newSocket(), _incomingMsg(), _serverMsg(buildResponse()), _hints(), _result(nullptr)
{
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(_ipAddress.c_str(), _port.c_str(), &_hints, &_result) != 0){
        std::cerr << "Couldn't get AddressInfo\n";
        exit(EXIT_FAILURE);
    }

    if (startServer() != 0){
        std::cerr << "For some reason we couldn't start the server\n";
    }
}

MyServer::~MyServer() {
    closeServer();
}

int MyServer::startServer() {
    _listenSocket = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol);
    if (_listenSocket < 0){
        perror("socket creation");
        printf("socket = %d\n", _listenSocket);
        exit(EXIT_FAILURE);
    }

    if (bind(_listenSocket, _result->ai_addr, _result->ai_addrlen) < 0){
        perror("Couldn't bind");
        close(_listenSocket);
        exit(EXIT_FAILURE);
    }
    setupEpoll();
    addToEpoll(_listenSocket, EPOLLIN);
    freeaddrinfo(_result);
    return (0);
}

void MyServer::startListening() {
    if (listen(_listenSocket, BACKLOG) < 0){
        perror("My ears are broken.");
        exit(EXIT_FAILURE);
    }

    int bytesRead;
    struct epoll_event events[MAX_EVENTS];

    int flags = fcntl(_newSocket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    flags |= O_NONBLOCK;
    if (fcntl(_newSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    while (1){
        std::cout << "========= Waiting for a new connection =========\n\n";
        int numEvents = epoll_wait(_epollFd, events, MAX_EVENTS, -1);
        std::cout << "num events = " << numEvents << std::endl;
        for (int i = 0; i < numEvents; i++) {
            if (events[i].data.fd == _listenSocket) {
                acceptConnection(_newSocket);
                addToEpoll(_newSocket, EPOLLIN|EPOLLET);
                char buffer[BUFFER_SIZE] = {0};
                bytesRead = read(_newSocket, buffer, BUFFER_SIZE);
                if (bytesRead < 0){
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                         perror("read error");
                         exit(EXIT_FAILURE);
                    }
                    std::cerr << "Couldn't read bytes from client connection\n";
                    exit(EXIT_FAILURE);
                }

                std::cout << "------ RECEIVED REQUEST ------\n\n";
                printf("%s", buffer);
                sendResponse();
                close(_newSocket);
            }
        }
    }
}

void MyServer::setupEpoll(){
    _epollFd = epoll_create1(0);
    if(_epollFd == -1){
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
}

void MyServer::addToEpoll(int fd, uint32_t events){
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void MyServer::acceptConnection(int &newSocket) {
    struct sockaddr_storage newSocketStuff;
    socklen_t newSocketStuffSize = sizeof(newSocketStuff);

    newSocket = accept(_listenSocket, (struct sockaddr *)&newSocketStuff, &newSocketStuffSize);
    if (newSocket < 0){
        std::cout << "Couldn't accept incoming connection\n";
        exit(EXIT_FAILURE);
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