#pragma once
#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "defines.hpp"

class Config {
	public:
		Config(const std::string &filePath);
		Config() = delete;
		Config(Config &other) = delete;
		Config operator=(const Config & other) = delete;
		~Config();

		const ConfigError&					getError() const;
		std::map<int, std::string>*			getErrorPages();
		const std::vector<ServerConfig>&	getServerConfigs() const;
		
		bool								hasErrorOccurred() const;
		std::string							buildErrorMessage(const ConfigError& err) const;

		void								printConfigs() const;

		class FileException : public std::exception {
			public:
				FileException(const std::string& msg);
				const char *what() const noexcept override;

			private:
				std::string _message;
		};

		class ParseException : public std::exception {
			public:
				ParseException(const std::string& msg);
				const char *what() const noexcept override;
			
			private:
				std::string _message;
		};

	private:
		std::string					_filePath;
		unsigned int				_lineNum;
		std::vector<ServerConfig>	_serverConfigs;
		ConfigError					_error;
		bool						_confErrorOccurred;

		void	_parseLine(const std::string& line, ServerConfig& config, std::ifstream& configFile);
		void	_handleErrorPages(ServerConfig& config, std::ifstream& configFile);
		void	_handleLocation(ServerConfig& config, std::ifstream& configFile, std::string& spec);
		void	_addConfig(const ServerConfig& config);
		void	_deleteConfig();
};

void		trim(std::string& s);
ConfigKey	resolveKey(const std::string& keyStr);

#endif
