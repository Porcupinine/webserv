/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   WebServ.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 12:55:16 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/19 20:26:24 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/WebServ.hpp"
#include "../../inc/VirtualHost.hpp"
#include "../../inc/Utils.hpp"
#include <fcntl.h>
#include <ctime>

// #include <regex> // check if needed

bool WebServ::_serverShutdown = false; 

WebServ::InitException::InitException(const std::string& msg) : _message(msg) {}

const char* WebServ::InitException::what() const noexcept {
	return _message.c_str();
}

WebServ::WebServ(int argc, char **argv) {
	try {
		if (argc != 2) {
			throw	InitException("argcount: hey, please provide me with a file!");
		}
		_setUpSigHandlers();

		std::string filePath = argv[1];
		Config config(filePath);
		if (config.hasErrorOccurred())
			throw	InitException(config.buildErrorMessage(config.getError()));
		config.printConfigs();  //Testing purposes.
		if ((_epollFd = epoll_create1(0)) == -1) {
			throw	std::runtime_error("epoll_create1: " + std::string(strerror(errno)));
		}
		_initializeServers(config);
	} catch (std::exception &e){
		std::string err_msg =  "error: " + std::string(e.what());
		throw InitException(err_msg);
	}
}

WebServ::~WebServ() {
	if (_epollFd != -1) close(_epollFd);

	_sharedPtrs_Servers.clear();
	_sharedPtrs_SharedData.clear();
	// _closeConnections();
}

// void WebServ::handleRequest(SharedData* shared) {
// 	std::cout << PURPLE << "Do I get here handleReq?" << RESET << std::endl;
// 	std::cout << "Received request:\n" << shared->request << std::endl;
// 	shared->response = "HTTP/1.1 200 OK\r\n"
// 	"Content-Type: text/plain\r\n"
// 	"Content-Length: 13\r\n"
// 	"\r\n"
// 	"Hello, world!";
// 	shared->status = Status::writing;
// }

void	WebServ::writeData(SharedData* shared) {
	std::cout << PURPLE << "Inside WriteData" << RESET << std::endl;
	if (shared->status == Status::closing) return;
	static int count;
	int clientFd = shared->fd;
	std::cout << PURPLE << "Am I here @start?\n" << RESET << std::endl;
	int len = std::min(static_cast<int>(shared->response.length()), BUFFER_SIZE);
	len = send(clientFd, shared->response.c_str(), len, MSG_NOSIGNAL);
	if (len == -1) {
		std::cerr << "Some error occured trying to send. Reason " << RED << strerror(errno) << RESET << std::endl;
		printf("\t\t%s%d%s", CYAN, clientFd, RESET);
		shared->status = Status::closing;
	} else if (len < static_cast<int>(shared->response.size()) && len != 0) {
		shared->response = shared->response.substr(len, shared->response.npos);
		shared->status = Status::writing;
	} else {
		std::cerr << "We wrote, now what? count " << RED << count++ << RESET << std::endl;
		shared->response.clear();
		shared->timestamp_last_request = std::time(nullptr);
		shared->status = shared->connection_closed ? Status::closing : Status::reading;
	}
}

// bool requestIsComplete(const std::string& buffer) {
// 	size_t headers_end = buffer.find("\r\n\r\n");
// 	if (headers_end == std::string::npos) {
// 		return false;
// 	}

// 	size_t content_length_pos = buffer.find("Content-Length:");
// 	if (content_length_pos != std::string::npos && content_length_pos < headers_end) {
// 		size_t start = buffer.find_first_of("0123456789", content_length_pos);
// 		if (start != std::string::npos) {
// 			size_t end = buffer.find(LINE_ENDING, start);
// 			if (end != std::string::npos) {
// 				int content_length = std::stoi(buffer.substr(start, end - start));
// 				size_t total_request_size = headers_end + 4 + content_length; // 4 for "\r\n\r\n"
// 				std::cout << total_request_size << "vs " << buffer.size() << std::endl;
// 				return buffer.size() >= total_request_size;
// 			}
// 		}
// 	} else {
// 		// No Content-Length header found, assume no body expected
// 		return true; // The headers are complete, and no body is expected
// 	}

// 	return false;
// }

void WebServ::readData(SharedData* shared) {
	char buffer[BUFFER_SIZE];
	int bytesRead;

	std::cout << PURPLE << "Inside ReadData" << RESET << std::endl;
	while (true) {
		bytesRead = recv(shared->fd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

		// std::cout << RED << " BYTES READ: "<< bytesRead << RESET << std::endl;
		// std::cout << buffer << std::endl;
		if (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			shared->request.append(buffer, bytesRead);
			shared->timestamp_last_request = std::time(&(shared->timestamp_last_request));
		} else if (bytesRead == 0) {
			// Connection closed by client
			shared->status = Status::closing;
			break;
		} else {
			if (shared->fd != -1) {
				shared->status = Status::handling_request;
				// shared->status = (requestIsComplete(buffer) ? Status::handling_request : Status::reading);
				break;
			} else {
				// Assuming an error that we need to handle as a critical failure
				std::cerr << "Critical error reading from socket.\n";
				shared->response_code = 500;
				shared->status = Status::handling_request;
			}
		}
	}

	if (shared->status == Status::handling_request) {
//		std::cerr << "fd = " << shared->fd << std::endl;
		std::cerr << "is this the one ? Request = " << shared->request << std::endl;
	}
}



void	WebServ::newConnection(SharedData* shared) {
	std::cout << PURPLE << "Inside newConnection?" << RESET << std::endl;
	int clientFd = accept(shared->fd, nullptr, nullptr);
	if (clientFd == -1) {
		std::cerr << "Failed to accept new connection: " << strerror(errno) << std::endl; //make this some exception
		return;
	}

	std::cout << PURPLE << "ClientFd: " << clientFd << "\tSharedFd: " << shared->fd << RESET << std::endl;
	_setNonBlocking(clientFd);

	std::cout << "Serv conf = " << shared->server_config->root_dir << std::endl;

	auto clientShared = std::make_shared<SharedData>();
	clientShared->fd = clientFd;
	clientShared->epoll_fd = shared->epoll_fd;
	clientShared->cgi_fd = -1;
	clientShared->cgi_pid = -1;
	clientShared->status = Status::reading;
	clientShared->request.clear();
	clientShared->response.clear();
	clientShared->response_code = 200;
	clientShared->server = shared->server;
	clientShared->server_config = shared->server_config;
	std::cout << shared->server_config->root_dir << std::endl;
	clientShared->connection_closed = false;
	clientShared->timestamp_last_request = std::time(nullptr);

	epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.ptr = clientShared.get();

	shared->timestamp_last_request = std::time(nullptr);
	if (epoll_ctl(shared->epoll_fd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
		std::cerr << "Error registering new client on epoll: " << strerror(errno) << std::endl;
		close(clientFd);
		return;
	}

	_sharedPtrs_SharedData.push_back(clientShared);
	std::cout << CYAN << "Registered client fd =" << clientFd << RESET << std::endl;
}

void	WebServ::run() {
	while (_serverShutdown == false) {
		int numEvents = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		ParseRequest req;
		for (int idx = 0; idx < numEvents; idx++) {
			SharedData* shared = static_cast<SharedData*>(_events[idx].data.ptr);
			_checkHangingSockets(shared);
			if (_events[idx].events & EPOLLIN && shared->status == Status::listening)
				newConnection(shared);
			if (_events[idx].events & EPOLLIN && shared->status == Status::reading)
				readData(shared);
			if (shared->status == Status::handling_request){
				// handleRequest(shared);
				std::cout << PURPLE << "Inside ParseReq" << RESET << std::endl;
				req = ParseRequest(shared);
				if (shared->status == Status::start_cgi)
					cgiHandler(shared, req);
				shared->request.clear();
			}
			if ((_events[idx].events & EPOLLHUP) && shared->status == Status::in_cgi){
				std::cout << "in WebServ cgi\n"; //TODO read from cgi fd here
				 readCGI(shared);
			}
			if ((_events[idx].events & EPOLLOUT) && shared->status == Status::writing)
				writeData(shared);
			if ((_events[idx].events & EPOLLERR) || shared->status == Status::closing) {
				std::cout << RED << "HERE" << RESET << std::endl;
				closeCGIfds(shared);
				closeConnection(shared);
				shared->request.clear(); // might introduce complications.. Guess well find out.
			}
		}
	}
	_closeConnections();
}

void	WebServ::_closeConnections() {
	int numEvents =  epoll_wait(_epollFd, _events, MAX_EVENTS, 0);
	for (int idx = 0; idx < numEvents; idx++) {
		SharedData *shared = static_cast<SharedData*>(_events[idx].data.ptr);
		if (_events[idx].events && shared->status != Status::listening) { // is de status check necessary?
			closeCGIfds(shared);
			closeConnection(shared);
		}
	}
}

void WebServ::_checkHangingSockets(SharedData *data) {
    double timeout = (data->status == Status::in_cgi) ? CGI_TIMEOUT : data->server->getTimeout();
    time_t currentTime = std::time(nullptr);
    double diff = std::difftime(currentTime, data->timestamp_last_request);

    // std::cout << "tmstamp = " << data->timestamp_last_request
    //           << " current time = " << currentTime
    //           << " server timeout = " << timeout
    //           << " diff = " << diff << std::endl;

    if (diff >= timeout) {
        std::cout << "Do I get in CheckHanging?" << std::endl;
        switch (data->status) {
            case Status::listening:
                std::cout << "for testing purposes.." << std::endl;
            //     data->response_code = 404;
				std::time(&(data->timestamp_last_request));
			// 	data->status = Status::handling_request;
                break;
            case Status::reading:
                std::cout << "reading.." << std::endl;
                data->response_code = 408;
				std::time(&(data->timestamp_last_request));
				data->status = Status::handling_request;
                break;
            case Status::handling_request:
                std::cout << CYAN << "handling_request.." << std::endl;
                data->response_code = 500; //..??
				data->status = Status::closing; // ../
                break;
            case Status::start_cgi:
                std::cout << "start_cgi.." << std::endl;
                data->response_code = 504;
				std::time(&(data->timestamp_last_request));
				data->status = Status::handling_request;
                if (data->cgi_fd != -1){
                    close(data->cgi_fd);
                    data->cgi_fd = -1;
                }
                if (kill(data->cgi_pid, 0) == 0) {
                    kill(data->cgi_pid, SIGTERM);
                }
                break;
			case Status::in_cgi: // Is this necessary?
				std::cout << "in_cgi.." << std::endl;
				// data->response_code = ...;
				break;
			case Status::writing:
				std::cout << "writing.." << std::endl;
				break;
			case Status::closing:
				std::cout << "CLOSING.." << std::endl;
				// closeConnection(data);
				break;
			// case Status::writing:
			// 	data->response_code = ...;
			// 	break;
		}
	}
}

void	WebServ::_setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);

	if (flags == -1) {
		std::string err_msg = "error getting flags: " + std::string(strerror(errno));
		throw InitException(err_msg);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::string err_msg = "error setting flags: " + std::string(strerror(errno));
		throw InitException(err_msg);
	}
}

std::vector<VirtualHost> WebServ::_setUpHosts(Config& conf) {
	std::vector<VirtualHost> virtualHosts;
	auto serverConfigs = conf.getServerConfigs();  // Now returns std::vector<std::shared_ptr<ServerConfig>>

	for (const auto& configPtr : serverConfigs) {
		VirtualHost vhost(configPtr->host, configPtr);
		virtualHosts.push_back(vhost);
	}
	return virtualHosts;
}

void WebServ::_initializeServers(Config& conf) {
	std::vector<VirtualHost>  virtualHosts = _setUpHosts(conf);
	for (const auto& vhost : virtualHosts) {
		std::shared_ptr<Server> server = std::make_shared<Server>();
		std::shared_ptr<ServerConfig> servConfig = std::shared_ptr(vhost.getConfig());

		// std::cout << servConfig.root_dir << std::endl;
		if (server->initServer(servConfig, _epollFd, SERVER_TIMEOUT, SERVER_MAX_NO_REQUEST) != 0) {
			throw InitException("Failed to initialize server for host: " + servConfig->host);
		}
		_servers.push_back(server);
		_sharedPtrs_Servers.push_back(server);
	}
}

void WebServ::_handleSignal(int sig) {
	if (sig == SIGINT)
		_serverShutdown = true;
	std::cout << RED << "Server Shutdown Started." << RESET << std::endl;
}

void WebServ::_setUpSigHandlers() {
	signal(SIGINT, _handleSignal);
}