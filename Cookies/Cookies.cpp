/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 13:18:59 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/13 13:52:56 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parseRequest.hpp"
#include "response.h"

// Set-Cookie: header is for when its a new one (within response)
// if its just Cookie: it just summerizes the content (within request)

/* read from request header */
std::map<std::string, std::string> parseRequest::parseCookies(const std::string& cookies) {
    std::map<std::string, std::string> cookieMap;
    std::istringstream cookieStream(cookies);
    std::string cookie;

    while (std::getline(cookieStream, cookie, ';')) {
        std::string::size_type pos = cookie.find('=');
        if (pos != std::string::npos) {
            std::string name = cookie.substr(0, pos);
            std::string value = cookie.substr(pos + 1);
            name.erase(0, name.find_first_not_of(' '));
            cookieMap[name] = value;
        }
    }
    return cookieMap;
}

std::string parseRequest::getAbsPath(void) const {
	return _absPathRoot;
}

/* create Set-Cookies for the response header */
std::vector<std::string> Response::createSetCookie(const std::map<std::string, std::string> &cookieMap) {
    std::vector<std::string> header;
    
    for (const auto& cookie : cookieMap) {
        std::ostringstream cookieStream;
        cookieStream << cookie.first << "=" << cookie.second;
        
        time_t now = time(0);
        now += 30 * 24 * 60 * 60; 
        cookieStream << "; Expires=" << getFormattedTime(now);

        cookieStream << "; Path=/; Secure; HttpOnly";
        header.push_back("Set-Cookie : " + cookieStream.str());
    }
    return header;
}

std::string Response::getFormattedTime(time_t rawtime) {
    struct tm *timeinfo;
    char buffer[80];
    
    timeinfo = gmtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    
    return std::string(buffer);
}
