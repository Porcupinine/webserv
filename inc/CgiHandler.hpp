/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:54:13 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:54:16 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CGI_HPP
#define CGI_HPP

int cgiHandler(SharedData* shared, ParseRequest& request);
//class CGI {
//private:
//public:
//	int cgiHandler(ParseRequest& request);
//};


#endif //CGI_HPP
