#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <cstring>
#include <vector>

// Functie om de server socket op te zetten
int setup_socket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Cannot create socket: " << strerror(errno) << std::endl;
        return -1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "Cannot set non-blocking: " << strerror(errno) << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return -1;
    }

    if (listen(sockfd, 10) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return -1;
    }

    return sockfd;
}

int main() {
    int port = 8080;
    int fd = setup_socket(port);
    if (fd < 0) {
        return 1;
    }

    std::vector<struct pollfd> fds;
    struct pollfd fd_struct;
    fd_struct.fd = fd;
    fd_struct.events = POLLIN;
    fds.push_back(fd_struct);

    while (true) {
        int ret = poll(fds.data(), fds.size(), -1); // Geen timeout, wacht oneindig
        if (ret < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); i++) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == fd) {
                    // Nieuwe verbinding accepteren
                    struct sockaddr_in client_addr;
                    socklen_t addr_len = sizeof(client_addr);
                    int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
                    if (client_fd < 0) {
                        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                        continue;
                    }

                    fcntl(client_fd, F_SETFL, O_NONBLOCK);  // Zet client socket op non-blocking
                    struct pollfd client_fd_struct = {client_fd, POLLIN, 0};
                    fds.push_back(client_fd_struct);
                } else {
                    // Lees data van client
                    char buffer[1024];
                    int nbytes = read(fds[i].fd, buffer, sizeof(buffer));
                    if (nbytes > 0) {
                        std::cout << "Received: " << std::string(buffer, nbytes) << std::endl;
                        // Echo de ontvangen data terug naar de client
                        write(fds[i].fd, buffer, nbytes);
                    } else {
                        if (nbytes < 0) {
                            std::cerr << "Read failed: " << strerror(errno) << std::endl;
                        }
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        i--;  // Pas de index aan na het verwijderen van een element
                    }
                }
            }
        }
    }

    close(fd);
    return 0;
}
