#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>
#include <netinet/in.h>
#include "Config.hpp"
#include "defines.hpp"

class Server {
public:
		Server();
		Server(Server const & src) = delete;
		Server &operator=(Server const & rhs) = delete; // Might still want this.
		~Server();

		int									initServer(ServerConfig *settings, int epollFd, double timeout, int maxNrOfRequests);
		void								setConnection(struct connection *conn);
		void								handleRequest(int clientFd);

		// Getter methods
		uint16_t							getPort() const;
		double								getTimeout() const;
		int									getMaxNrOfRequests() const;
		std::map<std::string, int>			getKnownClientIds() const;

		// Content getters
		std::string							getIndex(const std::string &host, const std::string &location) const;
		bool								getDirListing(const std::string &host, const std::string &location) const;
		std::map<int, std::string>			getRedirect(const std::string &host, const std::string &location) const;
		std::string							getRootFolder(const std::string &host, const std::string &location) const;
		size_t								getMaxBodySize(const std::string &host, const std::string &location) const;
		std::string							getUploadDir(const std::string &host, const std::string &location) const;
		std::set<std::string>				getAllowedMethods(const std::string &host, const std::string &location) const;

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
		struct connection*					_getConnection() const;
		ServerConfig*						_getHostConfigs(const std::string &host) const;

		void								_sendMockResponse(int clientFd);

		std::string							_inheritRootFolder(ServerConfig *hostSettings, const std::string &location) const;
		std::string							_inheritUploadDir(ServerConfig *hostSettings, const std::string &location) const;
		std::set<std::string>				_inheritAllowedMethods(ServerConfig *hostSettings, const std::string &location) const;
		std::string							_inheritIndex(ServerConfig *hostSettings, const std::string &location) const;
		size_t								_inheritMaxBodySize(ServerConfig *hostSettings, const std::string &location) const;

		int									_fd;
		struct sockaddr_in					_serverAddr;
		std::map<std::string, int>			_knownClientIds;
		double								_timeout;
		int									_maxNrOfRequests;
		std::list<ServerConfig *>			_configs;
		struct connection					*_conn;
};
