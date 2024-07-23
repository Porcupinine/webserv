#pragma once
#define WEBSERV_HPP

#include "Config.hpp"
#include "VirtualHost.hpp"
#include <sys/epoll.h>

#define MAX_EVENTS		20
#define BACKLOG		20

class WebServ {
	public:
		WebServ(int argc, char **argv);
		~WebServ();

		WebServ() = delete;
		WebServ(WebServ &other) = delete;
		WebServ operator=(const WebServ &other) = delete;

		void	run();
		void	stop();

		bool	getServerStatus() const;
		void	setServerStatus(bool status);
		void	handleRequest(int clientFd);

		class InitException : public std::exception {
			public:
				InitException(const std::string& msg);
				const char* what() const noexcept override;
			
			private:
				std::string	_message;
		};

	private:
		int					_epollFd;
		// int							_listenSocket;
		bool						_serverShutdown;
		struct epoll_event			events[MAX_EVENTS];
		std::list<Server>			_servers;

		static void					_handleSignal();
		void						_sendMockResponse(int clientFd);

		void						_setUpSigHandlers();
		void						_initializeServers(Config& conf);
		std::vector<virtualHost>	&_setUpHosts(Config &conf);
		
		// void						_setUpListenSocket();
		// void						_checkHanging(); Figure out some implementation for this. and an errorResponse.
		void						_closeConnections();

};
