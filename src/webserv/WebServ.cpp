#include "WebServ.hpp"

WebServ::WebServ(int argc, char **argv) : _serverShutdown(false) {
	try {
		if (argc != 2) {
			throw	WebServ::InitException("argcount: hey, please provide me with a file!");
		}
		_setUpSigHandlers();

		Config config(argv[1]);
		if (config.hasErrorOccurred())
			throw	WebServ::InitException(config.buildErrorMessage(config.getError()));
		config.printConfigs();
		if ((_epollFd = epoll_create()) == -1) {
			throw	std::runtime_error("epoll_create1: " + std::string(strerror(errno)));
		}
		_virtualHosts = _setUpHosts(config);
	} catch (std::exception &e){
		std::cout << "Error: " << e.what() << std::endl;
	}
}

// Dit moet naar connectionutils.cpp ofzo
void	closeCGIfds(SharedData* shared) {
	if (shared->cgi_fd != -1) {
		epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->cgi_fd, nullptr);
		if (close(shared->server_fd))
			std::cout << RED << "failed to close fd " << shared->server_fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
		shared->cgi_fd = -1;
	}
}

void	closeConnection(SharedData* shared) {
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->server_fd, ); // Add some delete I believe?
	if (close(shared->server_fd))
	std::cout << RED << "failed to close fd " << shared->server_fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
	delete shared; // double check this.
}

void	WebServ::_closeConnections() {
	int numEvents =  epoll_wait(_epollFd, _events, MAX_EVENTS, 0); // what does this do?
	for (int idx = 0; idx < numEvents; idx++) {
		SharedData *shared = static_cast<SharedData*>(_events[idx].data.ptr);
		if (_events[idx].events && shared->status != Status::listening) {
			closeCGIfds(shared); // still needs substance;
			closeConnection(shared);
		}
	}
}

void WebServ::handleRequest(SharedData* shared) {
    char buffer[1024];
	int clientFd = shared->server_fd;
    int bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received request:\n" << buffer << std::endl;
        _sendMockResponse(clientFd);
    } else {
        std::cerr << "Failed to read from client: " << strerror(errno) << std::endl;
    }
    close(clientFd);
}

void WebServ::_sendMockResponse(int clientFd) {
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, world!";
    send(clientFd, response, strlen(response), MSG_DONTWAIT);
}

void	WebServ::run() {
	while (_serverShutdown == false) {
		int numEvents = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		for (int idx = 0; idx < numEvents; idx++) {
			SharedData* shared = static_cast<SharedData*>(_events[idx].data.ptr);

			// _checkHanging(); Still need ot figure something here out.
			if (_events[idx].events & EPOLLIN && shared->status == Status::listening)
				newConnection(shared);
			// if (_events[idx].events & EPOLLIN && shared->status == Status::reading)
			// 	_readData(shared);
			if (shared->status == Status::reading) // just for now, i think?
				handleRequest(shared);
			// if (_events[idx].events & EPOLLHUP && shared->status == Status::in_cgi)
			// 	_readCGI(shared);
			// if (_events[idx].events & EPOLLOUT && shared->status == Status::writing)
			// 	_writeData(shared);
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

void WebServ::_initializeServers(Config& conf) {
	_virtualHosts = _setUpHosts(conf);
	for (const auto& vhost : _virtualHosts) {
		Server server;
		ServerConfig& servConfig = vhost.getConfigs();
		// if (server.initServer(&servConfig, _epollFd, servConfig.timeout, servConfig.maxNrOfRequests) != 0) {
		if (server.initServer(&servConfig, _epollFd, SERVER_TIMEOUT, SERVER_MAX_NO_REQUEST) != 0) {
			throw InitException("Failed to initialize server for host: " + servConfig.host);
		}
		_servers.push_back(server);
	}
}

std::vector<VirtualHost> WebServ::_setUpHosts(Config& conf) {
	std::vector<VirtualHost> virtualHosts;

	std::vector<ServerConfig> serverConfigs =  conf.getServerConfigs();
	for (const auto& configs : serverConfigs) {
		VirtualHost vhost(configs.host, configs);
		virtualHosts.push_back(vhost);
	}
}


sighandler_t WebServ::_handleSignal(int sig) {
	if (sig == SIGINT)
		_serverShutdown = true;
}

void WebServ::_setUpSigHandlers() {
	signal(SIGINT, _handleSignal());
}

WebServ::~WebServ() {
	if (_epollFd != -1) close(_epollFd);
	// if (_listenSocket != -1) close(_listenSocket); This needs to happen in hostDestructors.. I think
}