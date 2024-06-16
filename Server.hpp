#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>

#include <signal.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>

# include <string>


class Server {
    private:
        std::string _ipAddress;
        std::string _port;

        int _epollFd;

        int _listenSocket;
        int _newSocket;
        std::string _serverMsg;
        struct addrinfo *_result;

        void setupEpoll();
        void addToEpoll(int fd, uint32_t events);

        void acceptConnection(int &new_socket);
        std::string buildResponse();
        void sendResponse();

    public:
        Server(std::string ipAddress, std::string port);
        ~Server();

        int startServer();
        void run();
        void closeServer();
        // void startListening();
};

#endif