/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ReadFromCGI.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: laura <laura@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/20 08:00:47 by laura         #+#    #+#                 */
/*   Updated: 2024/08/20 08:00:47 by laura         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ParseRequest.hpp"
#include <unistd.h>
#include <string>
#include <filesystem>
#include <sys/epoll.h>

void readCGI(SharedData* shared) {
	ssize_t buffLen = 0;
	std::string response;
	std::string buffer(BUFFER_SIZE, '\0');
	while ((buffLen = ::read(shared->cgi_fd, buffer.data(), BUFFER_SIZE)) > 0) {
		response.append(buffer.c_str(), buffLen);
	}
	if (buffLen < 0) {
		std::cerr<<"Failed to read!\n";
	}
	std::cout<<"------------response-------------\n\'"<<response<<"\'\n";
	shared->response = response + "\0";
	shared->status = Status::writing;
	shared->connection_closed = true;
}

//TODO check if the script had problems executing??
