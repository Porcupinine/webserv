#include "VirtualHost.hpp"

VirtualHost::VirtualHost(const std::string& name, std::shared_ptr<ServerConfig> Config) : _name(name), _conf(Config) {}

const std::string& VirtualHost::getName() const { return _name; }
std::shared_ptr<ServerConfig> VirtualHost::getConfig() const { return _conf; }
