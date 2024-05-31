#include "webSocket.h"
#include <cstdlib>
#include <iostream>
#include "defines.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

namespace webserv {

	webSocket::webSocket(int ip, int port) : m_ip(ip), m_port(port) {
		m_serverAddress.sin_family = AF_INET;
		m_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); //any address
		m_serverAddress.sin_port = htons(m_port);
	}

	void webSocket::createSocket() {
		if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			std::cout << "Failed to creat socket: "<<strerror(errno)<<"\n";
			exit(1);
		}
		int opt = 1;
		if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
			0) {
			std::cout<< "Failed to set SO_REUSEADDR option: "<< strerror(errno) << "\n";
			exit(EXIT_FAILURE);
		}
		if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) <
			0) {
			std::cout << "Failed to set SO_REUSEPORT option: "<< strerror(errno) << "\n";
			exit(EXIT_FAILURE);
		}
		if (bind(m_socket, (sockaddr *) &m_serverAddress, sizeof(sockaddr)) <
			0) {
			std::cout << "Failed to bind socket: "<< strerror(errno)<<"\n";
			exit(1);
		}
		if (::listen(m_socket, N_CLIENTS) < 0) {
			std::cout << "Me deaf: "<< strerror(errno)<<"\n";
			exit(1);
		}
		addPoll(m_socket);
	}

	void webSocket::read() {
		int clonedSocket{};
		for (;;) {
			if (poll(m_pollFds.data(), m_pollFds.size(), -1) < 0) {
				std::cout << "Failed poll: " << strerror(errno) << "\n";
				exit(1);
			}
			for (auto &pollFd: m_pollFds) {
				if (pollFd.revents == 0)
					continue;
				int sd = pollFd.fd;
				if (pollFd.revents & POLLIN) {
					if (sd == m_socket) {
						std::cout << "Waiting for connection...\n";
						if ((clonedSocket = accept(m_socket, (sockaddr *) &m_serverAddress, &m_serverAddressLen)) < 0) {
							std::cout << "Can't be cloned: " << strerror(errno) << "\n";
							exit(1);
						}
						addPoll(clonedSocket);
						pollFd.revents = 0;
					} else {
						std::string message{0};
						ssize_t buff_len = 0;
						std::string buffer(BUFFER_SIZE, '\0');
						message.clear();
						std::cout << "Wait for message from [" << pollFd.fd << "]\n";
						while ((buff_len = ::read(pollFd.fd, buffer.data(), BUFFER_SIZE)) > 0) {
							message.append(buffer.data(), buff_len);
						}
						std::cout << "message: " << message << std::endl;
						if (buff_len < 0) {
							std::cout << "Can't read: " << strerror(errno) << "\n";
							exit(1);
						}
						respond(pollFd.fd);
						close(pollFd.fd);
					}
				}
			}
		}
	}





//			std::string buffer(BUFFER_SIZE, '\0');
//			std::cout << "Waiting for connection...\n";
//			if ((clonedSocket = accept(m_socket,(sockaddr *) &m_serverAddress,&m_serverAddressLen)) < 0) {
//				std::cout << "Can't be cloned: "<< strerror(errno)<<"\n";
//				exit(1);
//			}
//			addPoll(clonedSocket);
//			message.clear();
//			std::cout << "Wait for message from [" << clonedSocket << "]\n";
//			while ((buff_len = ::read(clonedSocket, buffer.data(),BUFFER_SIZE)) > 0) {
//				message.append(buffer.data(), buff_len);
//			}
//			std::cout << "message: " << message << std::endl;
//			if (buff_len < 0) {
//				std::cout << "Can't read: "<< strerror(errno)<<"\n";
//				exit(1);
//			}
//			respond(clonedSocket);
//			if (shutdown(clonedSocket, SHUT_RDWR) < 0) {
//				std::cout << "Failed to close socket: " << strerror(errno)<< "\n";
//				exit(1);
//			}
//		}
//	}

	void webSocket::respond(int socketd) {
		::write(socketd, "Yeah broh", 10);
	}

	void webSocket::addPoll(const int fd) {
		pollfd newPoll{};
		newPoll.fd = fd;
		newPoll.events = POLLIN;
		newPoll.revents = 0;
		m_pollFds.push_back(newPoll);
	}
}