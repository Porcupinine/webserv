#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <cstring>
# include <map>
# include <list>
# include <vector>
# include <stdexcept>
# include <netinet/in.h>
# include "Config.hpp"
# include "defines.hpp"

class Server {
public:
		Server();
		Server(Server const & src) = delete;
		Server &operator=(Server const & rhs) = delete; // Might still want this.
		~Server();

		int									initServer(const ServerConfig *settings, int epollFd, double timeout, int maxNrOfRequests);
		void								setConnection(SharedData *shared);
		// void								handleRequest(int clientFd);

		// Getter methods
		uint16_t							getPort() const;
		double								getTimeout() const;
		int									getMaxNrOfRequests() const;
		std::map<std::string, int>			getKnownClientIds() const;

		// Content getters
		std::string							getIndex(const std::string &location) const;
		bool								getDirListing(const std::string &location) const;
		std::map<int, std::string>			getRedirect(const std::string &location) const;
		std::string							getRootFolder(const std::string &location) const;
		size_t								getMaxBodySize() const;
		std::string							getUploadDir(const std::string &location) const;
		std::set<std::string>				getAllowedMethods(const std::string &location) const;
		const ServerConfig*					getConf();

		class ServerException : public std::exception {
		public:
				ServerException(const std::string& message) {
						_message = message + std::string(strerror(errno));
				}
				const char* what() const noexcept override {
						return _message.c_str();
				}
		private:
				std::string _message;
		};

private:
		int									_getFD() const;
		struct sockaddr_in					_getServerAddr() const;
		std::shared_ptr<SharedData>			_getSharedData() const;

		void								_bindSocket();
    	void								_listenSocket(int backlog);
		void								_setSocketOptions();
		void								_setSharedData();
		void								_registerWithEpoll(int epollFd, int fd, uint32_t events);

		int									_fd;
		struct sockaddr_in					_serverAddr;
		std::map<std::string, int>			_knownClientIds;
		double								_timeout;
		int									_maxNrOfRequests;
		const ServerConfig*					_configs;
		// std::list<ServerConfig *>			_configs;
		std::shared_ptr<SharedData>			_shared;
};

// Should be in utils.cpp
#endif
