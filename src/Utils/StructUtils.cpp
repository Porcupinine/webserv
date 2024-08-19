/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   structUtils.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 12:55:08 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/19 20:07:45 by ewehl         ########   odam.nl         */
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
	return (host == other->host && port == other->port /*&& server_name == other->server_name
			&& index == other->index && auto_index == other->auto_index && root_dir == other->root_dir
			&& upload_dir == other->upload_dir && max_client_body_size == other->max_client_body_size
			&& map_compare(error_pages, other->error_pages) && locations == other->locations*/);
}