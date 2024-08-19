/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHost.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:55:12 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:55:13 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/VirtualHost.hpp"

VirtualHost::VirtualHost(const std::string& name, std::shared_ptr<ServerConfig> Config) : _name(name), _conf(Config) {}

const std::string& VirtualHost::getName() const { return _name; }
std::shared_ptr<ServerConfig> VirtualHost::getConfig() const { return _conf; }
