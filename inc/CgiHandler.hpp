/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CgiHandler.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/08/19 12:54:13 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/20 12:43:30 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CGI_HPP
#define CGI_HPP

int cgiHandler(SharedData* shared, ParseRequest& request);
void writeCGI(SharedData* shared, ParseRequest& request);
void readCGI(SharedData* shared);
void finishCGI(SharedData *shared);


#endif //CGI_HPP
