#include "VirtualHost.hpp"

VirtualHost::VirtualHost(const std::string& name, const ServerConfig& Config) : _name(name), _conf(Config) {}

const std::string& VirtualHost::getName() const { return _name; }
const ServerConfig& VirtualHost::getConfig() const { return _conf; }
