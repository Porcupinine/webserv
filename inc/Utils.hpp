/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:54:31 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:54:32 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include "../inc/defines.hpp"

//      fdManagement.cpp
void	closeConnection(SharedData* shared);
void	closeCGIfds(SharedData* shared);

#endif 