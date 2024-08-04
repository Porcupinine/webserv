#include "Config.hpp"

Config::Config(const std::string& filePath) : _filePath(filePath), _lineNum(0), _confErrorOccurred(false) {
	
	size_t findExt = filePath.find_last_of(".");
    std::string ext = filePath.substr(findExt, sizeof("conf"));
    if ((findExt + 4) == filePath.size() && ext != "conf")
		throw FileException(WRONG_EXT);

	std::ifstream configFile(filePath);
	if (!configFile)
		throw FileException(OPEN_FILE_ERR);

	std::string						line;
	std::unique_ptr<ServerConfig>	currentConf;

	while (std::getline(configFile, line)) {
		_lineNum++;
		trim(line);
		if (line.empty() || line[0] == '#') continue;

		if (line == "server {") {
			currentConf = std::make_unique<ServerConfig>();
			continue;
		}

		if (line == "}") {
			if (currentConf) {
				std::cout << "adding config: " << currentConf->host << ":" << currentConf->port << std::endl;
				_serverConfigs.push_back(*currentConf);
				currentConf.reset();
			}
			continue;
		}

		if (currentConf) {
			try {
					_parseLine(line, *currentConf, configFile);
			} catch (const ParseException& e) {
				_confErrorOccurred = true;
				_error = {e.what(), static_cast<unsigned int>(_lineNum)};
			}
		}
	}
	if (currentConf) {
		_confErrorOccurred = true;
		throw ParseException("Incorrect serverblock");
	}
}

const std::vector<ServerConfig>& Config::getServerConfigs() const { return _serverConfigs; }

void	Config::_parseLine(const std::string& line, ServerConfig& config, std::ifstream& configFile) {
	std::istringstream	iss(line);
	std::string			keyStr, spec;

	iss >> keyStr;
	ConfigKey key = resolveKey(keyStr);

	switch(key) {
		case ConfigKey::host:
			// std::cout << "Hoest" << std::endl;
			iss >> config.host;
			break;
		case ConfigKey::port:
			// std::cout << "Poert" << std::endl;
			iss >> config.port;
			break;
		case ConfigKey::server_name:
			// std::cout << "sname" << std::endl;
			iss >> config.server_name;
			break;
		case ConfigKey::index:
			// std::cout << "index" << std::endl;
			iss >> config.index;
			break;
		case ConfigKey::auto_index:
			// std::cout << "aidx" << std::endl;
			iss >> config.auto_index;
			break;
		case ConfigKey::root_dir:
			// std::cout << "root" << std::endl;
			iss >> config.root_dir;
			break;
		case ConfigKey::upload_dir:
			// std::cout << "uload" << std::endl;
			iss >> config.upload_dir;
			break;
		case ConfigKey::max_client_body_size:
			// std::cout << "mcbs" << std::endl;
			iss >> config.max_client_body_size;
			break;
		case ConfigKey::error_pages:
			// std::cout << "errpages" << std::endl;
			_handleErrorPages(config, configFile);
			break;
		case ConfigKey::location:
			// std::cout << "lloc" << std::endl;
			iss >> spec;
			_handleLocation(config, configFile, spec);
			break;
		default:
			throw ParseException("Wot le fuq is this key: " + keyStr);
	}
}

void Config::_handleErrorPages(ServerConfig& config, std::ifstream& configFile) {
	std::string				line;

	while (getline(configFile, line)) {
		_lineNum++;
		trim(line);
		if (line.empty() || line[0] == '#') continue;
		if (line == "}") break;

		int					errorCode;
		std::string			errorPagePath;
		std::istringstream	iss(line);

		if (!(iss >> errorCode >> errorPagePath)) {
			throw ParseException(INVALID_ERROR_PAGE);
		}
		config.error_pages[errorCode] = errorPagePath;
	}
}

void Config::_handleLocation(ServerConfig& config, std::ifstream& configFile, std::string& spec) {
	std::string	line;
	Locations location;

	location.specifier = spec.substr(0, spec.find_first_of('{'));
	trim(location.specifier);
	while (getline(configFile, line)) {
		_lineNum++;
		trim(line);
		if (line.empty() || line[0] == '#') continue;
		if (line == "}") break;

		std::istringstream	iss(line);
		std::string			key;
		iss >> key;

		if (key == DIR_LISTING) {
			std::string val;
			iss >> val;
			location.dir_listing = (val == "on");
		} else if (key == LOCATIONS){
			iss >> location.path;
		} else if (key == ROOT_DIR) {
			iss >> location.root_dir;
		} else if (key == UPLOAD_DIR) {
			iss >> location.upload_dir;
		} else if (key == DEFAULT_FILE) {
			iss >> location.default_file;
		} else if (key == ALLOWED_METH) {
			std::string	method;
			while (iss >> method)
				location.allowed_methods.insert(method);
		} else if (key == REDIRECT) {
			int			statusCode;
			std::string	url;
			if (!(iss >> statusCode >> url))
				throw ParseException(INVALID_REDIR_FORMAT);
			location.redirect[statusCode] = url;
		} else {
			throw ParseException(UNKNOWN_KEY + key);
		}
	}
	config.locations.push_back(location);
}

bool Config::hasErrorOccurred() const { return _confErrorOccurred; }
const ConfigError& Config::getError() const { return _error; }

std::string Config::buildErrorMessage(const ConfigError& err) const {
	std::string fullMessage = "[" + std::to_string(err.fileNum) + "] " + err.message + "\n";

	return fullMessage;
}

void Config::printConfigs() const {
	std::cout << GREEN << "\nServerConfig count = " << _serverConfigs.size() << RESET << std::endl;

	for (const auto &config : _serverConfigs){
		std::cout << "host: " << config.host << std::endl;
		std::cout << "port: " << config.port << std::endl;
		std::cout << "server_name: " << config.server_name << std::endl<< std::endl;

		std::cout << "\tindex: " << config.index << std::endl;
		std::cout << "\tauto_index: " << ((config.auto_index == true )? "on" : "off") << std::endl << std::endl;
		std::cout << "\troot: " << config.root_dir << std::endl;
		std::cout << "\tupload_dir: " << config.upload_dir << std::endl << std::endl;
		std::cout << "\tmax_client_body_size: " << config.max_client_body_size << std::endl << std::endl;

		if (config.error_pages.size() > 0){
			std::cout << "\terror_pages {" << std::endl;
			for (const auto &error_page: config.error_pages) {
				std::cout << "\t\terror_page: [" << error_page.first << "] " << error_page.second << std::endl;
		}
		std::cout << "\t}" << std::endl << std::endl;
		}

		std::cout << "\tLocation Count = " << config.locations.size() << std::endl;
		for (const auto &location : config.locations) {
			std::cout << "\tspecifier: " << location.specifier << std::endl;
			std::cout << "\t\tpath: " << location.path << std::endl;
			std::cout << "\t\tdir_listing: " << ((location.dir_listing == true) ? "on" : "off") << std::endl;
			std::cout << "\t\troot: " << location.root_dir << std::endl;
			std::cout << "\t\tupload_dir: " << location.upload_dir << std::endl;
			std::cout << "\t\tdefault_file: " << location.default_file << std::endl;
			for (const auto &method : location.allowed_methods) {
				std::cout << "\t\tallow " << method << std::endl;
			}
			for (const auto &redir : location.redirect) {
				std::cout << "\t\treturn " << redir.first << " " << redir.second << std::endl;
			}
		}

		std::cout << SEPARATOR << std::endl << std::endl;
	}
}

// Before I call this function I need to check if config is alreaddy in there.
// Perhaps a operator== overload?
//		bool operator==(const ServerConfig& other);
void	Config::_addConfig(const ServerConfig& config) {
	_serverConfigs.push_back(config);
}

Config::~Config(){
	_serverConfigs.clear();
}

// utils.cpp

void	trim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [] (unsigned char c){
		return !std::isspace(c);
	}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){
		return !std::isspace(c);
	}).base(),s.end());
}

// Kinda cool, but is it really necessary..? No, not really.
ConfigKey resolveKey(const std::string& keyStr) {
	static const std::map<std::string, ConfigKey> keyMap = {
		{HOST, ConfigKey::host},
		{PORT, ConfigKey::port},
		{SERVER_NAME, ConfigKey::server_name},
		{INDEX, ConfigKey::index},
		{AUTO_INDEX, ConfigKey::auto_index},
		{ROOT_DIR, ConfigKey::root_dir},
		{UPLOAD_DIR, ConfigKey::upload_dir},
		{MAX_CLIENT_BODY_SIZE, ConfigKey::max_client_body_size},
		{ERROR_PAGES, ConfigKey::error_pages},
		{LOCATIONS, ConfigKey::location}
	};

	auto it = keyMap.find(keyStr);
	if (it != keyMap.end()){
		return it->second;
	}
	return ConfigKey::undefined;
}

// Exceptions:
Config::FileException::FileException(const std::string& msg) : _message("File error: " + msg) {}

const char* Config::FileException::what() const noexcept {
	return _message.c_str();
}

Config::ParseException::ParseException(const std::string& msg) : _message("Parser error: " + msg) {}

const char* Config::ParseException::what() const noexcept {
	return _message.c_str();
}