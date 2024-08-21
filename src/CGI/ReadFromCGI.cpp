/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ReadFromCGI.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/20 08:00:47 by laura         #+#    #+#                 */
/*   Updated: 2024/08/21 16:43:08 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// #include "../../inc/ParseRequest.hpp"
// #include <unistd.h>
// #include <string>
// #include <filesystem>
// #include <sys/epoll.h>
// #include <string.h>

// void writeCGI(SharedData* shared, ParseRequest& request) {
// 	auto body = request.getBodyMsg();
// 	if(write(shared->cgi_write, body.c_str(), body.size()) == -1){
// 		std::cerr << "Couldn't write to child, reason: " << strerror(errno) << "\n";
// 		shared->response_code = 504;
// 		shared->status = Status::handling_request;
// 		// if this happens we need to go to domi. err 500? or something gateway 504.
// 	}
// 	close(shared->cgi_write);
// 	std::cout<<"body done!\n";
// 	shared->status =  Status::in_cgi;
// }

// void readCGI(SharedData* shared) {
// 	ssize_t buffLen = 0;
// 	std::string response;
// 	std::string buffer(BUFFER_SIZE, '\0');
// 	while ((buffLen = read(shared->cgi_read, buffer.data(), BUFFER_SIZE)) > 0) {
// 		response.append(buffer.c_str(), buffLen);
// 	}
// 	if (buffLen < 0) {
// 		std::cerr<< "Couldn't read CGI response! " << strerror(errno) << "\n";
// 		// if this happens we need to go to domi. err 500? or something gateway 504.
// 		shared->response_code = 504;
// 		shared->status = Status::handling_request;
// 	}
// 	std::cout<<"------------response-------------\n\'"<<response<<"\'\n";
// 	shared->response = response + "\0";
// 	shared->status = Status::writing;
// 	shared->connection_closed = true;
// 	close(shared->cgi_read);
// }

// void finishCGI(SharedData *shared) {
// 	int status = 0; // This needs to be initialized.
// 	if (waitpid(shared->cgi_pid, &status, 0) == -1) {
// 		std::cerr << "Failed to wait for child process\n";
// 	} else if (WIFEXITED(status)) {
// 		std::cout << "Child exited with status " << WEXITSTATUS(status) << "\n";
// 	}
// }

//TODO check if the script had problems executing??
