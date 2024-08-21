/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   FdManagement.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 12:55:03 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/21 16:08:10 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include <sys/epoll.h>
#include <string.h>

void	closeCGIfds(SharedData* shared) {
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->cgi_read, nullptr); //fail check this..?
	if (shared->cgi_read != -1) {
		if (close(shared->cgi_read) == -1)
			std::cout << RED << "failed to close read_cgi fd " << shared->cgi_read << ": " << std::string(strerror(errno)) << RESET << std::endl;
		std::cout << GREEN << "successfully closed read_cgi fd " << shared->cgi_read << "." << RESET << std::endl;
		shared->cgi_read = -1;
	}
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->cgi_write, nullptr); //fail check this..?
	if (shared->cgi_write != -1) {
		if (close(shared->cgi_write) == -1)
			std::cout << RED << "failed to close write_cgi fd " << shared->cgi_write << ": " << std::string(strerror(errno)) << RESET << std::endl;
		std::cout << GREEN << "successfully closed write_cgi fd " << shared->cgi_write << "." << RESET << std::endl;
		shared->cgi_write = -1;
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
