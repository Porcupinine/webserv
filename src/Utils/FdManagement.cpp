/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fdManagement.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 12:55:03 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/19 20:08:34 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include <sys/epoll.h>
#include <string.h>

void	closeCGIfds(SharedData* shared) {
	if (shared->cgi_fd != -1) {
		epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->cgi_fd, nullptr); //fail check this..?
		if (close(shared->cgi_fd) == -1)
			std::cout << RED << "failed to close cgi fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
		shared->cgi_fd = -1;
	}
}

void	closeConnection(SharedData* shared) {
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->fd, nullptr); // fail check this..?
	if (shared->fd != -1) {
		if (close(shared->fd) == -1)
			std::cout << RED << "failed to close regular fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
		else
			std::cout << GREEN << "successfully closed regular fd " << shared->fd << "." << RESET << std::endl;
		shared->fd = -1;
	}
}
