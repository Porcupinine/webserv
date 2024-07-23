#include "Server.hpp"

Server::Server() : _fd(-1), _timeout(0.0), _maxNrOfRequests(0), _conn(nullptr) {
	// Initialize other necessary members
}

// Server::Server(Server const & src) {
//     // Copy constructor implementation if needed
// }

Server::~Server() {
	if (_fd != -1) {
		close(_fd);
	}
}

int Server::initServer(ServerConfig *config, int epollFd, double timeout, int maxNrOfRequests) {
	_timeout = timeout;
	_maxNrOfRequests = maxNrOfRequests;

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	// _serverAddr.sin_addr.s_addr = inet_addr(config->host.c_str()); voor de echte host?
	_serverAddr.sin_port = htons(config->port);

	// Create and bind socket
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		throw ServerException("Failed to create socket: ");
	}
	if (bind(_fd, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0) {
		throw ServerException("Failed to bind socket: ");
	}

	// Set up epoll
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _fd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _fd, &event) < 0) {
		throw ServerException("Failed to add socket to epoll: ");
	}

	// Additional initialization
	_configs.push_back(config);

	return 0;
}

void Server::setConnection(struct connection *conn) {
	_conn = conn;
}

uint16_t Server::getPort() const {
	return ntohs(_serverAddr.sin_port);
}

double Server::getTimeout() const {
	return _timeout;
}

int Server::getMaxNrOfRequests() const {
	return _maxNrOfRequests;
}

std::map<std::string, int> Server::getKnownClientIds() const {
	return _knownClientIds;
}

std::string Server::getIndex(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.index;
		}
	}
	return inheritIndex(config, location);
}

bool Server::getDirListing(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.dirListing;
		}
	}
	return false; // Default value
}

std::map<int, std::string> Server::getRedirect(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.redirect;
		}
	}
	return std::map<int, std::string>();
}

std::string Server::getRootFolder(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.root;
		}
	}
	return inheritRootFolder(configs, location);
}

size_t Server::getMaxBodySize(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.maxBodySize;
		}
	}
	return inheritMaxBodySize(configs, location);
}

std::string Server::getUploadDir(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.uploadDir;
		}
	}
	return inheritUploadDir(configs, location);
}

std::set<std::string> Server::getAllowedMethods(const std::string &host, const std::string &location) const {
	ServerConfig* configs = _getHostConfigs(host);
	if (configs) {
		auto it = configs->locations.find(location);
		if (it != configs->locations.end()) {
			return it->second.allowedMethods;
		}
	}
	return inheritAllowedMethods(configs, location);
}

// Private Methods

int Server::getFD() const {
	return _fd;
}

struct sockaddr_in Server::getServerAddr() const {
	return _serverAddr;
}

struct connection* Server::getConnection() const {
	return _conn;
}

ServerConfig* Server::_getHostConfigs(const std::string &host) const {
	for (ServerConfig* configs : _configs) {
		if (configs->host == host) {
			return configs;
		}
	}
	return nullptr;
}

// Methods to inherit settings from higher level if not specified

std::string Server::inheritRootFolder(ServerConfig *hostConfigs, const std::string &location) const {
	if (hostConfigs) {
		return hostConfigs->root; // Assuming root folder is set at the host level
	}
	return "/"; // Default root folder
}

std::string Server::inheritUploadDir(ServerConfig *hostConfigs, const std::string &location) const {
	if (hostConfigs) {
		return hostConfigs->uploadDir; // Assuming upload dir is set at the host level
	}
	return "/uploads"; // Default upload directory
}

std::set<std::string> Server::inheritAllowedMethods(ServerConfig *hostConfigs, const std::string &location) const {
	if (hostConfigs) {
		return hostConfigs->allowedMethods; // Assuming allowed methods are set at the host level
	}
	return {"GET", "POST"}; // Default allowed methods
}

std::string Server::inheritIndex(ServerConfig *hostConfigs, const std::string &location) const {
	if (hostConfigs) {
		return hostConfigs->index; // Assuming index is set at the host level
	}
	return "index.html"; // Default index file
}

size_t Server::inheritMaxBodySize(ServerConfig *hostConfigs, const std::string &location) const {
	if (hostConfigs) {
		return hostConfigs->maxBodySize;
	}
	return ONE_MB;
}
