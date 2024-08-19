/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:54:39 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:54:40 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "../inc/Config.hpp"
# include "../inc/Server.hpp"
# include "../inc/VirtualHost.hpp"
# include <signal.h>
# include <sys/epoll.h>
# include "../inc/ParseRequest.hpp"
# include "../inc/CgiHandler.hpp"

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

		void	newConnection(SharedData* shared);
		void	handleRequest(SharedData* shared);
		void	readData(SharedData* shared);
		void	writeData(SharedData* shared);
		// void	initErrorPages(SharedData* shared);

		class InitException : public std::exception {
			public:
				InitException(const std::string& msg);
				const char* what() const noexcept override;
			
			private:
				std::string	_message;
		};

	private:
		int											_epollFd;
		static bool									_serverShutdown;
		epoll_event									_events[MAX_EVENTS];
		std::vector<std::shared_ptr<Server>>		_servers;

		std::vector<std::shared_ptr<Server>>		_sharedPtrs_Servers;
		std::vector<std::shared_ptr<SharedData>>	_sharedPtrs_SharedData;

		static void									_handleSignal(int sig);
		// void										_sendMockResponse(int clientFd);

		void										_setUpSigHandlers();
		void										_initializeServers(Config& conf);
		std::vector<VirtualHost>					_setUpHosts(Config &conf);
		
		void										_setNonBlocking(int fd);

		// void										_checkHanging(); Figure out some implementation for this. and an errorResponse.
		void										_closeConnections();

};
#endif
