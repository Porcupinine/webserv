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

		Config config(argv[1]);
		if (config.hasErrorOccurred())
			throw	InitException(config.buildErrorMessage(config.getError()));
		// config.printConfigs();  Testing purposes.
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
		if (close(shared->fd))
			std::cout << RED << "failed to close fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
		shared->cgi_fd = -1;
	}
}

void	closeConnection(SharedData* shared) {
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->fd, nullptr); // Add some delete I believe?
	if (close(shared->fd))
	std::cout << RED << "failed to close fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
	delete shared; // double check this.
}

void	WebServ::_closeConnections() {
	int numEvents =  epoll_wait(_epollFd, _events, MAX_EVENTS, 0); // what does this do?
	for (int idx = 0; idx < numEvents; idx++) {
		SharedData *shared = static_cast<SharedData*>(_events[idx].data.ptr);
		if (_events[idx].events && shared->status != Status::listening) {
			// closeCGIfds(shared); // still needs substance;
			closeConnection(shared);
		}
	}
}

void WebServ::handleRequest(SharedData* shared) {
	char buffer[1024];
	int clientFd = shared->fd;
	int bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	std::cout << PURPLE << "Do I get here handleReq?" << RESET << std::endl;
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0';
		shared->request = buffer;
		std::cout << "Received request:\n" << buffer << std::endl;
		// _sendMockResponse(clientFd);
		shared->response = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 13\r\n"
		"\r\n"
		"Hello, world!";
		shared->status = Status::writing;
	} else {
		std::cerr << "Failed to read from client: " << strerror(errno) << std::endl;
	}
	// close(clientFd);
}

// void WebServ::_sendMockResponse(int clientFd) {
// 	const char *response =
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Type: text/plain\r\n"
// 		"Content-Length: 13\r\n"
// 		"\r\n"
// 		"Hello, world!";
// 	send(clientFd, response, strlen(response), MSG_DONTWAIT);
// }

void	WebServ::writeData(SharedData* shared) {
	int clientFd = shared->fd;
	int len = std::min(static_cast<int>(shared->response.length()), BUFFER_SIZE);
	len = send(clientFd, shared->response.c_str(), len, MSG_NOSIGNAL);
	if (len == -1) {
		std::cerr << "Some error occured trying to send." << std::endl;
	} else if (len < static_cast<int>(shared->response.size())) {
		shared->response = shared->response.substr(len, shared->response.npos);
		shared->status = Status::writing;
	} else {
		shared->response.clear();
		shared->status = shared->connection_closed ? Status::closing : Status::reading;
	}
}

void	WebServ::run() {
	while (_serverShutdown == false) {
		int numEvents = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		for (int idx = 0; idx < numEvents; idx++) {
			std::cout << PURPLE << "this works." << RESET << std::endl;
			printf("--------------- %p ----------------\n", _events[idx].data.ptr);
			SharedData* shared = static_cast<SharedData*>(_events[idx].data.ptr);
    		printf("--------------- %p ----------------\n", shared);
			// _checkHanging(); Still need to figure something out here.
			if (_events[idx].events & EPOLLIN && shared->status == Status::listening)
				newConnection(shared);
			// if (_events[idx].events & EPOLLIN && shared->status == Status::reading)
			// 	_readData(shared);
			if (shared->status == Status::reading) // just for now, i think? Or do I need another status?
				handleRequest(shared);
			// if (_events[idx].events & EPOLLHUP && shared->status == Status::in_cgi)
			// 	_readCGI(shared);
			if (_events[idx].events & EPOLLOUT && shared->status == Status::writing)
				writeData(shared);
			// if (_events[idx].events & EPOLLERR || shared->status == Status::closing) {
			// 	_closeCGIfds(shared);
			// 	_closeConnections(shared);
			// }
		}
	}

	_closeConnections();
}

void	WebServ::stop() {

}

void	WebServ::_setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error getting flags: " << strerror(errno) << std::endl;
		return;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "Error setting non-blocking: " << strerror(errno) << std::endl;
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

		if (server->initServer(&servConfig, _epollFd, SERVER_TIMEOUT, SERVER_MAX_NO_REQUEST) != 0) {
			throw InitException("Failed to initialize server for host: " + servConfig.host);
		}
		_servers.push_back(std::move(server));
	}
}

void	WebServ::newConnection(SharedData* shared) {
	int clientFd = accept(shared->fd, nullptr, nullptr);
	std::cout << PURPLE << "Do I get here newConnection?" << RESET << std::endl;
	if (clientFd == -1) {
		std::cerr << "Failed to accept new connection: " << strerror(errno) << std::endl; //make this some exception
		return;
	}

	_setNonBlocking(clientFd); // Add some errorhandling
	std::cout << PURPLE << "Do I get here newConnection?" << RESET << std::endl;

	std::unique_ptr<SharedData> clientShared(new SharedData);
	clientShared->fd = clientFd;
	clientShared->epoll_fd = shared->epoll_fd;
	clientShared->cgi_fd = 0;
	clientShared->cgi_pid = 0;
	clientShared->status = Status::reading;
	clientShared->request = "";
	clientShared->response = "";
	clientShared->server_config = shared->server_config;
	clientShared->connection_closed = false;
	clientShared->timestamp_last_request = std::time(nullptr);

	epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.ptr = clientShared;

	if (epoll_ctl(shared->epoll_fd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
		std::cerr << "Error registering new client on epoll: " << strerror(errno) << std::endl;
		close(clientFd);
		delete clientShared;
		return;
	}
	std::cout << CYAN << "Registered client fd =" << clientFd << RESET << std::endl;
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