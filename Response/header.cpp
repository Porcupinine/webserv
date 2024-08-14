/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 15:50:08 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/13 14:42:16 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.h"
#include "Server.hpp"

std::string Response::buildResponseHeader(parseRequest& request, struct SharedData* shared) {
    std::string header = "";
	initResponseHeaderFields();
	setHeaderValues(request);
	header = getHeaderValues(request, header, shared);
	return header;
}

void Response::initResponseHeaderFields() {
	_allow = "";
	_contentLanguage = "";
	_contentLength = 0;
	_contentType = "";
	_date = "";
	_location = "";
}

/* HEADER SETTERS */
void Response::setHeaderValues(parseRequest& request) {
	_allow = setAllow();
	_contentLanguage = request.getLanguageStr();
	_contentLength = _response.size();
	std::cout << "setting contentLen = " << _response.size() << std::endl; // to rm
	_contentType = _type; // but what if empty??
	_date = setDate();
	_location = "";
	_setcookies.clear();
	if (request.getCookies().size() > 0)
		_setcookies = createSetCookie(request.getCookies());
}

std::string Response::setAllow() {
	std::ostringstream allowedMethods;

    for (auto it = _method.begin(); it != _method.end(); ++it) {
        allowedMethods << it->first;
        if (std::next(it) != _method.end())
            allowedMethods << ", ";
    }
    return allowedMethods.str();
}

std::string Response::setDate() {
	std::time_t now = std::time(nullptr);
	std::tm* gmt = std::gmtime(&now);

	std::stringstream dateStream;
	dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
	return dateStream.str();
}


/* HEADER GETTERS */
std::string Response::getHeaderValues(parseRequest& request, std::string header, struct SharedData* shared) {
	header += "HTTP/" + request.getVersion() + " " + std::to_string(_statusCode) + " " + getMatchingCodeString(_statusCode) + LINE_ENDING;
	if (_allow != "")
		header += "Allow: " + _allow + LINE_ENDING;
	if (request.getCookies().size() > 0) {
		for (const auto& cookie : _setcookies)
			header += cookie + "\r\n";
	}
	if (_contentLanguage != "")
		header += "Content-Language: " + _contentLanguage + LINE_ENDING;
	if (_contentLength != 0)
		header += "Content-Length: " + std::to_string(_contentLength) + LINE_ENDING;
		if (_contentType != "")
		header += "Content-Type: " + _contentType + LINE_ENDING;
	if (_date != "")
		header += "Date: " + _date + LINE_ENDING;
	if (_statusCode == 301 || _statusCode == 302 || _statusCode == 307 || _statusCode == 308 || request.getRedirection() == true){
		// int key = 1;
		std::cout << "HERE IN LOCATION HEADER\n";
		auto redirectMap = shared->server->getRedirect(request.getPath()); // Hier moet dus de location name in.
		// auto redirectMap = shared->server->getRedirect(shared->server_config->locations->path); // Hier moet dus de location name in..
		auto it = redirectMap.find(_statusCode);
		if (it != redirectMap.end()) {
       		header += "Location: " + it->second + "\r\n";
    	}
		// auto it = shared->server_config->locations[0].redirect.find(key);
		// if (it != shared->server_config->locations[0].redirect.end()) {
		// 	header += "Location: " + it->second + LINE_ENDING;
		// }
	}
	header += "Connection: closed\r\n";

	if (_response != "")
		header += LINE_ENDING + _response + LINE_ENDING;
	// std::cout << "header = " << header << std::endl; 
	return header;
}

std::string Response::getMatchingCodeString(unsigned int code) {
    std::map<unsigned int, std::string>::iterator it = _errorCodes.find(code);

    if (it == _errorCodes.end())
        return ("Error"); // or something else??
    else
        return (it->second);
}
