/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 15:50:08 by dmaessen          #+#    #+#             */
/*   Updated: 2024/07/24 14:24:26 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.h"

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
	_contentLength = "";
	_contentType = "";
	_date = "";
	_location = "";
}

/* HEADER SETTERS */
void Response::setHeaderValues(parseRequest& request) {
	_allow = setAllow(request);
	_contentLanguage = request.getLanguageStr();
	_contentLength = _response.size();
	_contentType = _type; // but what if empty??
	_date = setDate(request);
	_location = "";
	_setcookies.clear();
	if (request.getCookies().size() > 0)
		_setcookies = createSetCookie(request.getCookies());
}

std::string Response::setAllow(parseRequest& request) {
	std::ostringstream allowedMethods;

    for (auto it = _method.begin(); it != _method.end(); it++) {
        allowedMethods << it->first;
        if (it != _method.end())
            allowedMethods << ", ";
    }
    return allowedMethods.str();
}

std::string Response::setDate(parseRequest& request) {
	std::time_t now = std::time(nullptr);
	std::tm* gmt = std::gmtime(&now);

	std::stringstream dateStream;
	dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
	return dateStream.str();
}


/* HEADER GETTERS */
std::string Response::getHeaderValues(parseRequest& request, std::string header, struct SharedData* shared) {
	header += request.getVersion() + " " + std::to_string(_statusCode) + " " + getMatchingCodeString(_statusCode) + "/r/n";
	if (_allow != "")
		header += "Allow: " + _allow + "/r/n";
	if (request.getCookies().size() > 0) {
		for (const auto& cookie : _setcookies)
			header += cookie + "\r\n";
	}
	if (_contentLanguage != "")
		header += "Content-Language: " + _contentLanguage + "/r/n";
	if (_contentLength != "")
		header += "Content-Length: " + _contentLength + "/r/n";
		if (_contentType != "")
		header += "Content-Type: " + _contentType + "/r/n";
	if (_date != "")
		header += "Date: " + _date + "/r/n";
	if (_statusCode == 301 || _statusCode == 302 || _statusCode == 307 || _statusCode == 308)
		header += "Location: " + shared->server_config->locations->redirection.second + "/r/n";
	header += "Connection: closed/r/n";

	if (_response != "")
		header += "/r/n" + _response + "/r/n";
	return header;
}

std::string Response::getMatchingCodeString(unsigned int code) {
    std::map<unsigned int, std::string>::iterator it = _errorCodes.find(code);

    if (it == _errorCodes.end())
        return ("Error"); // or something else??
    else
        return (it->second);
}
