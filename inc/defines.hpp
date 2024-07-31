#pragma once

#ifndef DEFINES_HPP
# define DEFINES_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <memory>

#include <ctime>

#include <sstream>
#include <fstream>

#include "parseRequest.hpp"

// MAGIC NUMBERS
#define ONE_MB					1048576 // Default max body size (1MB)
#define BUFFER_SIZE				1024
#define SERVER_TIMEOUT			2000
#define MAX_EVENTS				20
#define BACKLOG					20
#define SERVER_MAX_NO_REQUEST	5

// COLORS
#define RESET	"\033[0m"
#define RED		"\033[31;1m"
#define GREEN	"\033[32;1m"
#define YELLOW	"\033[33;1m"
#define BLUE	"\033[34;1m"
#define PURPLE	"\033[35;1m"
#define CYAN	"\033[36;1m"

// SERVER CONFIG
#define HOST					"host"
#define PORT					"port"
#define INDEX					"index"
#define LOCATIONS				"location"
#define AUTO_INDEX				"autoindex"
#define SERVER_NAME				"server_name"
#define ERROR_PAGES				"error_pages"
#define MAX_CLIENT_BODY_SIZE	"max_client_body_size"

// LOCATIONS
#define REDIRECT				"return"
#define DIR_LISTING				"dir_listing"
#define CGI_HANDLERS			"cgi_handlers"
#define DEFAULT_FILE			"default_file"
#define ALLOWED_METH			"allow"

// MUTUAL
#define ROOT_DIR				"root"
#define UPLOAD_DIR				"upload_dir"

#define UNKNOWN_KEY				"Unknown key found "
#define OPEN_FILE_ERR			"Couldn't open file"
#define INVALID_REDIR_FORMAT	"invalid format for redirect"
#define INVALID_ERROR_PAGE		"invalid error page format"
#define SEPARATOR				"********************************************************"

//Enum class > enum, because of typesafety, readability (scoped access, so you know exactly what its used for.), also reducing risk of namecollisions.
enum class ConfigKey {
	host, port, server_name, index, auto_index, root_dir, upload_dir,
		max_client_body_size, error_pages, location, undefined
};

enum class Status	{
	listening, reading, handling_request, in_cgi, writing, closing
};

struct Locations {
	bool								dir_listing;

	std::string							path;
	std::string							specifier;
	std::string							root_dir;
	std::string							upload_dir;
	std::string							default_file;

	std::set<std::string>				allowed_methods;
	std::map<int, std::string>			redirect; // Domi had hier een vraag over, but I don't remember..
	// std::map<std::string, std::string>	cgi_handlers; // ?? I think this is necessary?
};

struct ServerConfig {
	std::string						host;
	uint16_t						port;
	std::string						server_name;

	std::string						index;
	bool							auto_index;

	std::string						root_dir;
	std::string						upload_dir;
	std::string						cgi_dir;
	size_t							max_client_body_size;

	std::map<int, std::string>		error_pages;
	std::vector<struct Locations>	locations;
};

struct ConfigError {
	std::string			message;
	unsigned int		fileNum;
};

struct SharedData {
	int								cgi_fd;
	pid_t							cgi_pid;

	std::string						request;
	std::string						response;
	int								response_code;

	int								fd;
	int								epoll_fd; // check new conection or initserver

	Status							status; // does this work like this? iT does :D
	// std::list<ServerConfig *>		server_config;
	const ServerConfig*				server_config;
	bool							connection_closed;

	time_t							timestamp_last_request;
};


#endif