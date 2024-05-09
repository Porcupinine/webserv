#include "myServer.hpp"

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

MyServer::MyServer(std::string ipAddress, std::string port) : _ipAddress(ipAddress), _port(port), _listenSocket(),
    _newSocket(), _incomingMsg(),/*_serverMsg(buildResponse()),*/ _hints(), _result(nullptr)
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
                handleResponse(buffer);
                // printf("%s", buffer);
                // sendResponse();
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

std::stringstream setMethod(char *buffer){
    std::stringstream ss(buffer);

    std::string firstLine;
    std::getline(ss, firstLine);

    std::cout << "The first line is: " << firstLine << std::endl;

    std::stringstream lineStream(firstLine);
    return lineStream;
}

void MyServer::handleResponse(char *buffer) {
    
    std::stringstream methodLine = setMethod(buffer);
    std::string method, uri, protocol;
    methodLine >> method >> uri >> protocol; // Dit kan beter, maar weet nog niet hoe.
    
    std::cout << "Method: " << method << std::endl;
    std::cout << "URI: " << uri << std::endl;
    std::cout << "Protocol: " << protocol << std::endl;

    std::string serverMessage = buildResponse(uri, protocol);
    if (serverMessage.empty()){
        std::cout << "Couldn't build a proper message\n";
        return;
    }
    sendResponse(serverMessage);
}

std::string getExtension(const std::string& uri){
    size_t lastDot = uri.find_last_of('.');
    if (lastDot != std::string::npos && (lastDot + 1) < uri.size())
        return uri.substr(lastDot+1);
    std::cout << "no dot..?\n";
    return "";
}

bool isValidExtension(const std::string& uriExt){
    std::string extensions[] = {"html", "php", "py"};
    for (int idx = 0; idx < 3; idx++){
        if (extensions[idx] == uriExt)
            return true;
    }
    return false;
}

std::string getFilePath(const std::string& uri) {
    std::string basePath = "../www";
    std::string fullPath;
    if (uri == "/"){
        fullPath = basePath + "/html/index.html";
    } else {           
        std::string uriExt = getExtension(uri);
        std::cout << uriExt + "\n";
        if (isValidExtension(uriExt) == false) return ""; // werk hier met exceptions?
        fullPath = basePath + "/" + uriExt + uri;
    }

    return fullPath;
}

std::string getFileAsStream(std::string &uri) {
    std::string filePath = getFilePath(uri);

    std::cout << filePath + "\n";

    std::ifstream file(filePath, std::ios::binary | std::ios::ate); // Open for reading at end to get size
    if (!file) {
        std::cerr << "hierzo..\n";
        filePath = getFilePath("/nope.html");
        std::cout << filePath + "\n";
        file.clear();
        file.open(filePath, std::ios::binary | std::ios::ate);
        if (!file){
            std::cout << "Hierzo dan?\n";
            std::cout << "Couldn't do file related stuff\n";
            return "";
        }
    }

    std::streamsize size = file.tellg();
    if (size < 0) {std::cout << size<< " we found the culprit\n"; exit (-2);}
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        // return ""; // or handle file read error
        exit(-1);
    }

    std::string content = std::string(buffer.begin(), buffer.end());
    // std::cout << content + "\n";
    return content;
    //find file in directory, depending on extension (?)
    //make file into valid stream? then return filestream/stringstream as .str();
}

std::string MyServer::buildResponse(std::string &uri, std::string &protocol) {
    std::string fileContents = getFileAsStream(uri);

    if (fileContents.empty())
        return "";

    std::ostringstream ss;
    ss << protocol << "200 OK\nContent-Type: text/html\nContent-Length: " << fileContents.size() << "\n\n" << fileContents; // Need to build this whole response.

    return ss.str();
}

void MyServer::sendResponse(std::string &serverMsg) {
    long bytesSent;

    bytesSent = write(_newSocket, serverMsg.c_str(), serverMsg.size());
    if (bytesSent == serverMsg.size()){
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