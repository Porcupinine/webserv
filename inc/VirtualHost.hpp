#pragma once
#ifndef VIRTUAL_HOST_HPP
# define VIRTUAL_HOST_HPP

# include "defines.hpp"

class VirtualHost {
public:
	VirtualHost(const std::string& name, std::shared_ptr<ServerConfig> config);

	const std::string& getName() const;
	std::shared_ptr<ServerConfig> getConfig() const;

private:
	std::string						_name;
	std::shared_ptr<ServerConfig>	_conf;
};
#endif
