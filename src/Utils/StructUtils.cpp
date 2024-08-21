/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   StructUtils.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 12:55:08 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/21 16:54:53 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/defines.hpp"

template<typename Map>
bool map_compare(Map const& lhs, Map const& rhs) {
    return (lhs.size() == rhs.size()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin(),
            [] (auto a, auto b) {return a.first == b.first;}));
}

bool Locations::operator==(const Locations& other) const {
	return (dir_listing == other.dir_listing && path == other.path && specifier == other.specifier
			&& root_dir == other.root_dir && upload_dir == other.upload_dir && default_file == other.default_file
			&& allowed_methods == other.allowed_methods && map_compare(redirect, other.redirect));
}

// Don't need to compare all the other things, I think
bool ServerConfig::operator==(const std::unique_ptr<ServerConfig>& other) const {
	return (host == other->host && port == other->port);
}

Locations* ServerConfig::getSpecifier(const std::string &path) const {
    if (locations.empty() == false)  {
        auto it = std::find_if(locations.begin(), locations.end(),
            [&path](const std::shared_ptr<struct Locations>& loc) {
                const std::string& specifier = loc->specifier;
                return (path == specifier || 
                        (path.find(specifier) == 0 && path[specifier.length()] == '/'));
            });
        if (it != locations.end()) {
            return it->get();
        }
    }
    return nullptr;
}


Locations* ServerConfig::getLocation(std::string &locationSpec) const {
	if (locations.empty() == false) {
		auto it = std::find_if(locations.begin(), locations.end(),
			[locationSpec](std::shared_ptr<struct Locations> const& loc) {
				return (loc->specifier == locationSpec); });
		if (it != locations.end()) {
			return it->get();
		}
	}
	return nullptr;
}

std::string ServerConfig::getIndex(const std::string &location) const {
	if (locations.empty() == false) {
		auto it = std::find_if(locations.begin(), locations.end(),
			[location](std::shared_ptr<struct Locations> const& loc) { return loc->specifier == location; });
		if (it != locations.end()) {
			if (it->get()->default_file.empty() == false)
				return it->get()->default_file;
		}
		return index;
	}
	return "index.html";
}

bool ServerConfig::getDirListing(const std::string &location) const {
	if (locations.empty() == false) {
		auto it = std::find_if(locations.begin(), locations.end(),
			[location](std::shared_ptr<struct Locations> const& loc) { return loc->specifier == location; });
		if (it != locations.end()) {
			return it->get()->dir_listing;
		}
	}
	return false; // Dit had ik afgesproken met Domi, right?
}

std::string ServerConfig::getRootFolder(const std::string &location) const {
	if (locations.empty() == false) {
		auto it = std::find_if(locations.begin(), locations.end(),
			[location](std::shared_ptr<struct Locations> const& loc) { return loc->specifier == location; });
		if (it != locations.end()) {
			return it->get()->root_dir.empty() ? root_dir : it->get()->root_dir; // Use location-specific or fallback to general
		}
	}
	return root_dir;
}

std::set<std::string> ServerConfig::getAllowedMethods(const std::string &location) const {
	if (locations.empty() == false) {
		auto it = std::find_if(locations.begin(), locations.end(),
			[location](std::shared_ptr<struct Locations> const& loc) { return loc->specifier == location; });
		if (it != locations.end() && !it->get()->allowed_methods.empty()) {
			return it->get()->allowed_methods;
		}
	}
	return {"GET", "POST"};
}

std::map<int, std::string> ServerConfig::getRedirect(const std::string &location) const {
	if (locations.empty() == false) {
		auto it = std::find_if(locations.begin(), locations.end(),
			[location](std::shared_ptr<struct Locations> const& loc) {return loc->specifier == location;});
		if (it != locations.end()) {
			if (!it->get()->redirect.empty()){
				// std::cout << RED << it->get()->redirect.begin()->first << RESET << std::endl;
				return it->get()->redirect;
			}
		}
	}
	return std::map<int, std::string>();
}

std::string ServerConfig::getUploadDir(const std::string &location) const {
	if (locations.empty() == false) {
		for (auto it = locations.rbegin(); it != locations.rend(); ++it) {
			if (location.find(it->get()->specifier) == 0) {
				if (!it->get()->upload_dir.empty()) {
					return it->get()->upload_dir;
				}
			}
		}
		if (upload_dir.empty() == false) {
			return upload_dir;
		}
	}
	return "/uploads";
}
