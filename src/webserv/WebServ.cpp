#include "WebServ.hpp"

WebServ::WebServ(int argc, char **argv) : _serverShutdown(false) {
	try {
		if (argc != 2) {
			throw	std::InitException("argcount: hey, please provide me with a file!");
		}
		_setUpSigHandlers();

		Config config(argv[1]);
		if (config.hasErrorOccurred())
			throw	std::InitException(config.buildErrorMessage(config.getError()));
		config.printConfigs();
		if ((_epollFd = epoll_create1()) == -1) {
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
		if (close(shared->fd))
			std::cout << RED << "failed to close fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
		shared->cgi_fd = -1;
	}
}

void	closeConnection(SharedData* shared) {
	epoll_ctl(shared->epoll_fd, EPOLL_CTL_DEL, shared->server_fd);
	if (close(shared->server_fd))
	std::cout << RED << "failed to close fd " << shared->fd << ": " << std::string(strerror(errno)) << RESET << std::endl;
	delete shared; // double check this.
}

void	WebServ::_closeConnections() {
	int numEvents =  epoll_wait(_epollFd, _events, MAX_EVENTS, 0); // what does this do?
	for (int idx = 0; idx < numEvents; idx++) {
		SharedData *shared = static_cast<SharedData*>(events[i].data.ptr);
		if (_events[i].events && shared->status != LISTENING) {
			closeCGIfds(shared); // still needs substance;
			closeConnection(shared);
		}
	}
}

void WebServ::handleRequest(int clientFd) {
    char buffer[1024];
    int bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received request:\n" << buffer << std::endl;
        sendMockResponse(clientFd);
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
    write(clientFd, response, strlen(response));
}

void	WebServ::run() {
	while (_serverShutdown == false) {
		int numEvents = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		for (int idx = 0; idx < numEvents; idx++) {
			SharedData* shared = static_cast<SharedData*>(_events[idx].data.ptr);

			_checkHanging();
			if (_events[i].events & EPOLLIN && shared->status == Status::listening)
				_newConnection(_epollFd, shared->server_fd, shared->server);
			// if (_events[i].events & EPOLLIN && shared->status == Status::reading)
			// 	_readData(shared);
			if (shared->status == HANDLING)
				_handleRequest(_epollFd, shared);
			// if (_events[i].events & EPOLLHUP && shared->status == Status::in_cgi)
			// 	_readCGI(_epollFd, shared);
			// if (_events[i].events & EPOLLOUT && shared->status == Status::writing)
			// 	_writeData(shared);
			// if (_events[i].events & EPOLLERR || shared->status == Status::closing) {
			// 	_closeCGIfds(_epollFd, shared);
			// 	_closeConnection(_epollFd, shared);
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
		const ServerSettings& settings = vhost.getConfigs();
		if (server.initServer(&settings, _epollFd, settings.timeout, settings.maxNrOfRequests) != 0) {
			throw InitException("Failed to initialize server for host: " + settings.host);
		}
		_servers.push_back(server);
	}
}

std::vector<VirtualHost> WebServ::_setUpHossts(Config& conf) {
	std::vector<VirtualHost> virtualHosts;

	std::vector<ServerConfig> serverConfigs =  conf.getServerConfigs();
	for (const auto& configs : serverConfigs) {
		VirtualHost vhost(configs.host, configs);
		virtualHost.push_back(vhost);
	}
}


void WebServ::_handleSignal(int sig) {
	if (sig == SIG_INT)
		_serverShutdown = true;
}

WebServ::_setUpSigHandlers() {
	signal(SIG_INT, _handleSignal());
}

WebServ::~WebServ() {
	if (_epollFd != -1) close(_epollFd);
	// if (_listenSocket != -1) close(_listenSocket); This needs to happen in hostDestructors.. I think
}