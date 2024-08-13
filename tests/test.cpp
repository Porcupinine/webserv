

#include <iostream>
#include <string>
#include <cstdint>

#include <arpa/inet.h>

// int main (int argc, char **argv){
//     if (argc != 2)
//         return 1;
//     unsigned long myres = std::stoul(argv[1]);
//     uint16_t port = static_cast<uint16_t>(myres);
//     std::cout << port << std::endl;
//     uint16_t what = htons(port);
//     std::cout << what << std::endl;

//     unsigned short sin_port = htons(8080);
//     std::cout << sin_port << std::endl;
    
//     return (0);
// }

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>

int fd, epoll_fd;

void handle_sigint(int sig) {
    std::cout << "Terminating server" << std::endl;
    close(fd);
    close(epoll_fd);
    exit(0);
}

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    signal(SIGINT, handle_sigint);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // Use IPv4
    hints.ai_socktype = SOCK_STREAM; // Use TCP
    hints.ai_flags = AI_PASSIVE;     // For wildcard IP address

    // Resolve the address and port
    int status = getaddrinfo(NULL, "8080", &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create socket file descriptor
    if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == 0) {
        perror("socket failed");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        freeaddrinfo(res);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the network address and port
    if (bind(fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind failed");
        freeaddrinfo(res);
        close(fd);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res); // Free the address info structure

    // Start listening for incoming connections
    if (listen(fd, 3) < 0) {
        perror("listen");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Add fd to epoll instance
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");
        close(fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            close(fd);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_fds; ++i) {
            if (events[i].data.fd == fd) {
                int new_socket = accept(fd, NULL, NULL);
                if (new_socket == -1) {
                    perror("accept");
                    continue;
                }

                set_nonblocking(new_socket);

                struct epoll_event new_event;
                new_event.events = EPOLLOUT | EPOLLET;
                new_event.data.fd = new_socket;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &new_event) == -1) {
                    perror("epoll_ctl");
                    close(new_socket);
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

    close(fd);
    close(epoll_fd);
    return 0;
}
