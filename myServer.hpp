#ifndef MYSERVER_HPP
# define MYSERVER_HPP

# include <iostream>

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>

# include <string>


class MyServer {
    private:
        std::string _ipAddress;
        std::string _port;

        int _epollFd;

        int _listenSocket;
        int _newSocket;
        long _incomingMsg;
        std::string _serverMsg;
        struct addrinfo _hints, *_res, *_result;
        
        int startServer();
        void closeServer();

        void setupEpoll();
        void addToEpoll(int fd, uint32_t events);

        void acceptConnection(int &new_socket);
        std::string buildResponse();
        void sendResponse();

    public:
        MyServer(std::string ipAddress, std::string port);
        ~MyServer();

        void startListening();
};

#endif