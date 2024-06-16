#include "Server.hpp"

#include <fstream>
#include <vector>
#include <sstream>

#include <string.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#include <stdlib.h>

#define BACKLOG     1
#define MAX_EVENTS 5
#define BUFFER_SIZE 30000

Server::Server(std::string ipAddress, std::string port) : _ipAddress(ipAddress), _port(port), _listenSocket(),
    _newSocket(), /* _serverMsg(buildResponse()) ,*/ _result(nullptr)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (int status = getaddrinfo(_ipAddress.c_str(), _port.c_str(), &hints, &_result) != 0){
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE); // Throws some exception
    }

    if (startServer() != 0){
        std::cerr << "For some reason we couldn't start the server\n";
    }
}

Server::~Server() {
    closeServer();
}

int Server::startServer() {
    int opt = 1;
    _listenSocket = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol);
    if (_listenSocket < 0){
        perror("socket creation");
        freeaddrinfo(_result);
        // printf("socket = %d\n", _listenSocket);
        exit(EXIT_FAILURE); // Throws some exception
    }

    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt");
        Server::closeServer();
        exit(EXIT_FAILURE); // Throws some exception
    }

    if (bind(_listenSocket, _result->ai_addr, _result->ai_addrlen) < 0){
        perror("Couldn't bind");
        freeaddrinfo(_result);
        Server::closeServer(); // Am I supposed to close the server now?
        exit(EXIT_FAILURE); // Throws some exception
    }
    setupEpoll();
    addToEpoll(_listenSocket, EPOLLIN);
    freeaddrinfo(_result);
    _result = nullptr;
    return (0);
}

void Server::run() {
    if (listen(_listenSocket, BACKLOG) < 0){
        perror("My ears are broken.");
        //Still need to clean up epoll skizz
        exit(EXIT_FAILURE); // Throws some exception
    }

    int bytesRead;
    struct epoll_event events[MAX_EVENTS];

    int flags = fcntl(_newSocket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE); // Throws some exception
    }

    flags |= O_NONBLOCK;
    if (fcntl(_newSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE); // Throws some exception
    }

    while (1){
        std::cout << "========= Waiting for a new connection =========\n\n";
        int numEvents = epoll_wait(_epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1){
            perror("epoll_wait");
            Server::closeServer();
            exit(EXIT_FAILURE); // Throws some exception
        }
        std::cout << "num events = " << numEvents << std::endl;
        for (int i = 0; i < numEvents; i++) {
            if (events[i].data.fd == _listenSocket) {
                acceptConnection(_newSocket);
                addToEpoll(_newSocket, EPOLLIN|EPOLLET);
            }else if (events[i].events & EPOLLIN) {
                char buffer[1024];
                printf("%s", buffer);
                ssize_t bytes_read = read(events[i].data.fd, buffer, sizeof(buffer) - 1);
                if (bytes_read == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("read");
                        close(events[i].data.fd);
                    }
                } else if (bytes_read == 0) {
                    // Connection closed by client
                    close(events[i].data.fd);
                } else { // Not sure this is necessary..
                    buffer[bytes_read] = '\0';
                    std::cout << "Received: " << buffer << std::endl;

                    // Prepare to send a response
                    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
                    struct epoll_event new_event;
                    new_event.events = EPOLLOUT | EPOLLET;
                    new_event.data.fd = events[i].data.fd;

                    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, events[i].data.fd, &new_event) == -1) {
                        perror("epoll_ctl");
                        close(events[i].data.fd);
                    }
                }
            } else if (events[i].events & EPOLLOUT) {
                const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
                ssize_t bytes_sent = send(events[i].data.fd, hello, strlen(hello), MSG_DONTWAIT);

                if (bytes_sent == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("send");
                        close(events[i].data.fd);
                    }
                } else {
                    // Data sent successfully
                    close(events[i].data.fd);
                }
            }
        }
    }
}

void Server::setupEpoll(){
    _epollFd = epoll_create1(0);
    if(_epollFd == -1){
        perror("epoll_create1");
        exit(EXIT_FAILURE); // Throws some exception
    }
}

void Server::addToEpoll(int fd, uint32_t events){
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE); // Throws some exception
    }
}

void Server::acceptConnection(int &newSocket) {
    struct sockaddr_storage newSocketStuff;
    socklen_t newSocketStuffSize = sizeof(newSocketStuff);

    newSocket = accept(_listenSocket, (struct sockaddr *)&newSocketStuff, &newSocketStuffSize);
    if (newSocket < 0){
        std::cout << "Couldn't accept incoming connection\n";
        exit(EXIT_FAILURE); // Throws some exception
    }
}

// std::string Server::buildResponse()
// {
//     std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> NOW WE KNOW :) </p></body></html>";
//     std::ostringstream ss;
//     ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
//        << htmlFile;

//     return ss.str();
// }

// void Server::sendResponse()
// {
//     ssize_t bytesSent;

//     while (bytesSent = send(_newSocket, _serverMsg.c_str(), _serverMsg.size(), MSG_DONTWAIT) < 0){
//         // Is this allowed? This is not a write || read; it's a send.. 
//         // Checking the value of errno is strictly forbidden after a read or a write operation.
//         if (errno == EAGAIN || errno == EWOULDBLOCK){
//             usleep(1000);
//         } else {
//             perror("send");
//             exit(EXIT_FAILURE); // Throws some exception
//         }
//     }
// }

void Server::closeServer() {
    if (_result != nullptr){
        freeaddrinfo(_result);
        _result = nullptr;  
    } if (_listenSocket != -1){
        close(_listenSocket);
        _listenSocket = -1;
    }
    if (_newSocket != -1){
        close(_newSocket);
        _newSocket = -1;
    }
}