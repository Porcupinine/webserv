#include "WebServ.hpp"
#include "VirtualHost.hpp"
#include <fcntl.h>
#include <ctime>

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
		std::cout << "Error: " << e.what() << std::endl;
	}
}

// Dit moet naar connectionutils.cpp ofzo
void	closeCGIfds(SharedData* shared) {
	if (shared->cgi_fd != -1) {
		epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->cgi_fd, nullptr);
		if (close(shared->cgi_fd) == -1)
			std::cout << RED << "failed to close cgi fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
		shared->cgi_fd = -1;
	}
}

void	closeConnection(SharedData* shared) {
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->fd, nullptr);
	if (close(shared->fd) == -1)
		std::cout << RED << "failed to close regular fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
}

void WebServ::handleRequest(SharedData* shared) {
	int clientFd = shared->fd;
	std::cout << PURPLE << "Do I get here handleReq?" << RESET << std::endl;
	std::cout << "Received request:\n" << shared->request << std::endl;
	shared->response = "HTTP/1.1 200 OK\r\n"
	"Content-Type: text/plain\r\n"
	"Content-Length: 13\r\n"
	"\r\n"
	"Hello, world!";
	shared->status = Status::writing;
}

void WebServ::readData(SharedData* shared) {
	char buffer[BUFFER_SIZE];
	int bytesRead;

	while (true) {
		bytesRead = recv(shared->fd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

		if (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			shared->request.append(buffer);
			std::time(&(shared->timestamp_last_request));
		} else if (bytesRead == 0) {
			// Connection closed by client
			shared->status = Status::closing;
			break;
		} else {
			if (shared->fd != -1) {
				shared->status = Status::handling_request;
				break;
			} else {
				// Assuming an error that we need to handle as a critical failure
				std::cerr << "Critical error reading from socket.\n";
				shared->response_code = 500;
				shared->status = Status::writing;
				auto pos = shared->errorPages.find(shared->response_code);
				if (pos != shared->errorPages.end()) {
					shared->response = pos->second;
				}
				break;
			}
		}
	}

	if (shared->status == Status::handling_request) {
		std::cerr << "fd = " << shared->fd << std::endl;
		std::cerr << "Request = " << shared->request << std::endl;
	}
}


void	WebServ::writeData(SharedData* shared) {
	if (shared->status == Status::closing) return;
	static int count;
	int clientFd = shared->fd;
	std::cout << PURPLE << "Am I here @start?\n" << RESET << std::endl;
	// std::cout << "resp = " << shared->response << std::endl;
	int len = std::min(static_cast<int>(shared->response.length()), BUFFER_SIZE);
	len = send(clientFd, shared->response.c_str(), len, MSG_NOSIGNAL);
	if (len == -1) {
		std::cerr << "Some error occured trying to send. Reason " << RED << strerror(errno) << RESET << std::endl;
		printf("\t\t%s%d%s", CYAN, clientFd, RESET);
		shared->status = Status::closing;
	} else if (len < static_cast<int>(shared->response.size())) {
		shared->response = shared->response.substr(len, shared->response.npos);
		shared->status = Status::writing;
	} else {
		std::cerr << "Does this ever happen? count " << RED << count++ << RESET << std::endl;
		shared->response.clear();
		shared->status = shared->connection_closed ? Status::closing : Status::reading;
	}
	std::cout << PURPLE << "Am I here?\n" << RESET << std::endl;
}

void	WebServ::_closeConnections() {
	int numEvents =  epoll_wait(_epollFd, _events, MAX_EVENTS, 0);
	for (int idx = 0; idx < numEvents; idx++) {
		SharedData *shared = static_cast<SharedData*>(_events[idx].data.ptr);
		if (_events[idx].events && shared->status != Status::listening) {
			closeCGIfds(shared);
			closeConnection(shared);
		}
	}
}

void	WebServ::run() {
	while (_serverShutdown == false) {
		int numEvents = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		parseRequest req;
		for (int idx = 0; idx < numEvents; idx++) {
			SharedData* shared = static_cast<SharedData*>(_events[idx].data.ptr);
			// _checkHanging(); Still need to figure something out here.
			if (_events[idx].events & EPOLLIN && shared->status == Status::listening)
				newConnection(shared);
			if (_events[idx].events & EPOLLIN && shared->status == Status::reading)
				readData(shared);
			if (shared->status == Status::handling_request){
				// handleRequest(shared);
				std::cout << "SHARED->SERVCONFIG" << std::endl;
				std::cout << shared->server_config->root_dir << std::endl;
				req = parseRequest(shared);
			}
			if ((_events[idx].events & EPOLLHUP) && shared->status == Status::in_cgi)
				cgiHandler(shared, req);
			if ((_events[idx].events & EPOLLOUT) && shared->status == Status::writing)
				writeData(shared);
			if ((_events[idx].events & EPOLLERR) || shared->status == Status::closing) {
				std::cout << RED << "HERE" << std::endl;
				closeCGIfds(shared);
				closeConnection(shared);
			}
		}
	}
	_closeConnections();
}


void	WebServ::stop() {

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

	std::vector<ServerConfig> serverConfigs =  conf.getServerConfigs();
	for (const auto& configs : serverConfigs) {
		VirtualHost vhost(configs.host, configs);
		virtualHosts.push_back(vhost);
	}
	return virtualHosts;
}

void WebServ::_initializeServers(Config& conf) {
	std::vector<VirtualHost>  virtualHosts = _setUpHosts(conf);
	for (const auto& vhost : virtualHosts) {
		auto server = std::make_unique<Server>();
		const ServerConfig& servConfig = vhost.getConfig();


		std::cout << servConfig.root_dir << std::endl;
		if (server->initServer(&servConfig, _epollFd, SERVER_TIMEOUT, SERVER_MAX_NO_REQUEST) != 0) {
			throw InitException("Failed to initialize server for host: " + servConfig.host);
		}
		_servers.push_back(std::move(server));
	}
}

void	WebServ::newConnection(SharedData* shared) {
	std::cout << PURPLE << "Do I get here newConnection?" << RESET << std::endl;
	int clientFd = accept(shared->fd, nullptr, nullptr);
	if (clientFd == -1) {
		std::cerr << "Failed to accept new connection: " << strerror(errno) << std::endl; //make this some exception
		return;
	}

	// printf("%sClientFd: %d\tSharedFd: %d%s\n", PURPLE, clientFd, shared->fd, RESET);
	std::cout << PURPLE << "ClientFd: " << clientFd << "\tSharedFd: " << shared->fd << RESET << std::endl;
	_setNonBlocking(clientFd); // Add some errorhandling

	auto clientShared = std::make_shared<SharedData>();
	clientShared->fd = clientFd;
	clientShared->epoll_fd = shared->epoll_fd;
	clientShared->cgi_fd = -1;
	clientShared->cgi_pid = -1;
	clientShared->status = Status::reading;
	clientShared->request = "";
	clientShared->response = "";
	clientShared->response_code = 200; // TODO check on this
	// clientShared->server = shared->server;
	clientShared->server_config = shared->server_config;
	std::cout << clientShared->server_config->root_dir << std::endl;
	// std::cout << "IN LOU " << shared->server_config->root_dir << " and " << shared->server_config->auto_index << "\n";
	clientShared->connection_closed = false;
	clientShared->timestamp_last_request = std::time(nullptr);
	initErrorPages(shared);

	epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.ptr = clientShared.get();

	if (epoll_ctl(shared->epoll_fd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
		std::cerr << "Error registering new client on epoll: " << strerror(errno) << std::endl;
		close(clientFd);
		return;
	}

	_sharedPtrs.push_back(clientShared);
	std::cout << CYAN << "Registered client fd =" << clientFd << RESET << std::endl;
}

void WebServ::initErrorPages(SharedData* shared) {
	shared->errorPages[301] = "HTTP/1.1 301 Moved Permanently\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 150\r\n\r\n "
	"<!DOCTYPE html><html><head><title>301</title></head><body><h1> 301 Moved Permanently! </h1><p>This page has been moved permanently.</p></body></html>";
	shared->errorPages[302] = "HTTP/1.1 302 Found\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 138\r\n\r\n "
	"<!DOCTYPE html><html><head><title>302</title></head><body><h1> 302 Found! </h1><p>This page has been temporarily moved.</p></body></html>";
	shared->errorPages[307] = "HTTP/1.1 307 Temporary Redirect\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 137\r\n\r\n "
	"<!DOCTYPE html><html><head><title>307</title></head><body><h1> 307 Temporary Redirect! </h1><p>This page is temporary.</p></body></html>";
	shared->errorPages[308] = "HTTP/1.1 308 Permanent Redirect\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 151\r\n\r\n "
	"<!DOCTYPE html><html><head><title>308</title></head><body><h1> 308 Permanent Redirect! </h1><p>This page has been permanently moved.</p></body></html>";
	shared->errorPages[400] = "HTTP/1.1 400 Bad Request\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 151\r\n\r\n "
	"<!DOCTYPE html><html><head><title>400</title></head><body><h1> 400 Bad Request Error! </h1><p>We are not speaking the same language!</p></body></html>";
	shared->errorPages[403] = "HTTP/1.1 403 Forbiden\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 130\r\n\r\n "
	"<!DOCTYPE html><html><head><title>403</title></head><body><h1> 403 Forbiden! </h1><p>This is top secret, sorry!</p></body></html>";
	shared->errorPages[404] = "HTTP/1.1 404 Not Found\r\n"
							"Content-Type: text/html\r\n\nContent-Length: 115\r\n\r\n "
							"<!DOCTYPE html><html><head><title>404</title></head><body><h1> 404 Page not found! </h1><p>Puff!</p></body></html>";
	// shared->errorPages[404] = "HTTP/1.1 404 Not Found\r\n\n"
	// "Content-Type: text/html\r\n\nContent-Length: 115\r\n\r\n "
	// "<!DOCTYPE html><html><head><title>404</title></head><body><h1> 404 Page not found! </h1><p>Puff!</p></body></html>";
	shared->errorPages[405] = "HTTP/1.1 405 Method Not Allowed\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 139\r\n\r\n "
	"<!DOCTYPE html><html><head><title>405</title></head><body><h1> 405 Method Not Allowed! </h1><p>We forgot how to do that!</p></body></html>";
	shared->errorPages[413] = "HTTP/1.1 413 Payload Too Large\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 163\r\n\r\n "
	"<!DOCTYPE html><html><head><title>413</title></head><body><h1> 413 Payload Too Large! </h1><p>We are too busy right now, please try again later!</p></body></html>";
	shared->errorPages[500] = "HTTP/1.1 500 Internal Server Error\r\n\n"
	"Content-Type: text/html\r\n\nContent-Length: 146\r\n\r\n "
	"<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
}

void WebServ::_handleSignal(int sig) {
	if (sig == SIGINT)
		_serverShutdown = true;
	std::cout << RED << "Server Shutdown Started." << RESET << std::endl;
}

void WebServ::_setUpSigHandlers() {
	signal(SIGINT, _handleSignal);
}

WebServ::~WebServ() {
	if (_epollFd != -1) close(_epollFd);
	// if (_listenSocket != -1) close(_listenSocket); This needs to happen in hostDestructors.. I think
}