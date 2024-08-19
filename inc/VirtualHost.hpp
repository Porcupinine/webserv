/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:54:34 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:54:35 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef VIRTUAL_HOST_HPP
# define VIRTUAL_HOST_HPP

# include "../inc/defines.hpp"

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
