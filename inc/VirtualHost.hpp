// #pragma once
// #ifndef VIRTUAL_HOST
// # define VIRTUAL_HOST

// # include <sys/socket.h> // For socket functions
// # include <netinet/in.h> // For sockaddr_in
// # include <cstdlib> // For exit() and EXIT_FAILURE
// # include <iostream> // For cout
// # include <unistd.h> // For read

// # include "defines.hpp"

// class VirtualHost {
// 	private:
// 		uint16_t								_port;

// 		int										_listenSocket;
// 		struct sockaddr_in						_address;

// 		struct ServerConfig						_settings;
// 		std::unique_ptr<struct SharedData>		*_shared; // is this how this works?

// 	public:

// };

// #endif

#pragma once

#include "defines.hpp"

class VirtualHost {
public:
	VirtualHost(const std::string& name, const ServerConfig& Config);

	const std::string& getName() const;
	const ServerConfig& getConfig() const;

private:
	std::string		_name;
	ServerConfig	_conf;
};
