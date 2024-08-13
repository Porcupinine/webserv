/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 15:49:40 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/13 11:58:40 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.h"

Response::Response(void) {
}

Response::~Response() {
}

Response&	Response::operator=(const Response &cpy) {
	this->_version = cpy.getVersion();
	this->_statusCode = cpy.getStatusCode();
    this->_response = cpy._response;
    this->_type = cpy._type;
    this->_isAutoIndex = cpy._isAutoIndex;
	return (*this);
}

/* PROCESS RESPONSE */
std::string Response::giveResponse(parseRequest& request, struct SharedData* shared) {
    _statusCode = request.getRetVal();
    _type = "";

    // std::cout << "test\t" << shared->server_config->auto_index << "\n" << shared->server_config->host << std::endl;
    _isAutoIndex = shared->server_config->auto_index;
    // _absrootpath = shared->server_config->root_dir;
    initErrorCodes();
    initMethods();
    if (shared->server_config->max_client_body_size < request.getBodyMsg().size())
        _statusCode = 413;
        
    std::map<std::string, ResponseCallback>::iterator it = _method.find(request.getMethod());
    if (it != _method.end())
        (this->*(it->second))(request, shared);
    else
        _statusCode = 405;
    if (_statusCode == 405 || _statusCode == 413) {
        _response = errorHtml(_statusCode);
        _response = buildResponseHeader(request, shared);
    }
    
    return _response;
}

/* STATIC INIT */
std::map<std::string, Response::ResponseCallback> Response::initMethods()
{
	std::map<std::string, ResponseCallback> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;
	return map;
}

std::map<std::string, Response::ResponseCallback> Response::_method = Response::initMethods();


/* METHOD FUNCTIONS */
void Response::getMethod(parseRequest& request, struct SharedData* shared) {
    if (_statusCode == 200) {
        readContent(request);
        _response = buildResponseHeader(request, shared);
    }
    else
        _response = errorHtml(_statusCode);
}

void Response::postMethod(parseRequest& request, struct SharedData* shared) {
    if (cgiInvolved(request.getPath()) == false) {
        _statusCode = 204; // no content
        _response = "";
        _response = buildResponseHeader(request, shared);
    }
    if (_statusCode == 500) {
        _response = errorHtml(_statusCode);
        _response = buildResponseHeader(request, shared);
    }
}

void Response::deleteMethod(parseRequest& request, struct SharedData* shared) {
    _response = "";

    if (fileExists(request.getPath()) == true){
        if (remove(request.getPath().c_str()) == 0)
            _statusCode = 204; // meaning no content, returned to indicate success and there is no body message
        else
            _statusCode = 403;
    }
    else
        _statusCode = 404;

    if (_statusCode == 404 || _statusCode == 403)
        _response = errorHtml(_statusCode);
    _response = buildResponseHeader(request, shared);
}

void Response::initErrorCodes()
{
	_errorCodes[100] = "Continue";
	_errorCodes[200] = "OK";
	_errorCodes[201] = "Created";
	_errorCodes[204] = "No Content";
    _errorCodes[301] = "Moved Permanently";
    _errorCodes[302] = "Found";
    _errorCodes[307] = "Temporary Redirect";
    _errorCodes[308] = "Permanent Redirect";
	_errorCodes[400] = "Bad Request";
	_errorCodes[403] = "Forbidden";
	_errorCodes[404] = "Not Found";
	_errorCodes[405] = "Method Not Allowed";
	_errorCodes[413] = "Payload Too Large";
	_errorCodes[500] = "Internal Server Error";
}

void Response::htmlErrorCodesMap() {
    _errorCodesHtml[301] = "<!DOCTYPE html><html><head><title>301</title></head><body><h1> 301 Moved Permanently! </h1><p>This page has been moved permanently</p></body></html>";
    _errorCodesHtml[302] = "<!DOCTYPE html><html><head><title>302</title></head><body><h1> 302 Found! </h1><p>This page has been temporarily moved</p></body></html>";
    _errorCodesHtml[307] = "<!DOCTYPE html><html><head><title>307</title></head><body><h1> 307 Temporary Redirect! </h1><p>This page is temporary</p></body></html>";
    _errorCodesHtml[308] = "<!DOCTYPE html><html><head><title>308</title></head><body><h1> 308 Permanent Redirect! </h1><p>This page has been permanently moved=</p></body></html>";
    _errorCodesHtml[400] = "<!DOCTYPE html><html><head><title>400</title></head><body><h1> 400 Bad Request Error! </h1><p>We are not speaking the same language!</p></body></html>";
    _errorCodesHtml[403] = "<!DOCTYPE html><html><head><title>403</title></head><body><h1> 403 Forbiden! </h1><p>This is top secret, sorry!</p></body></html>";
    _errorCodesHtml[404] = "<!DOCTYPE html><html><head><title>404</title></head><body><h1> 404 Page not found! </h1><p>Puff!</p></body></html>";
    _errorCodesHtml[405] = "<!DOCTYPE html><html><head><title>405</title></head><body><h1> 405 Method Not Allowed! </h1><p>We forgot how to do that!</p></body></html>";
    _errorCodesHtml[413] = "<!DOCTYPE html><html><head><title>413</title></head><body><h1> 413 Payload Too Large! </h1><p>We are too busy right now, please try again later!</p></body></html>";
    _errorCodesHtml[500] = "<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
    if (_statusCode == 301  || _statusCode == 302 || _statusCode == 307 || _statusCode == 308 ||
    _statusCode == 400 || _statusCode == 403 || _statusCode == 404 ||
    _statusCode == 405 || _statusCode == 413 || _statusCode == 500)
        _type = "text/html";
}


/* HTML RELATED */
std::string Response::errorHtml(unsigned int error) {
    std::map<unsigned int, std::string>::iterator it = _errorCodesHtml.find(error);

    if (it == _errorCodesHtml.end())
        return ("<!DOCTYPE html><body><h1> 404 </h1><p> Error Page Not Found </p></body></html>");
    else
        return (it->second);
}

void Response::readContent(parseRequest& request) {
    std::ifstream file;

    if (fileExists(request.getPath()) == true) {
        file.open((request.getPath().c_str()), std::ifstream::in);
        if (!file.is_open()) {
            _statusCode = 403;
            _response = errorHtml(_statusCode);
            return ; // or break ??
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        _response = content;
        file.close();
    }
    else if (_isAutoIndex == true) {
        std::stringstream buffer;
        buffer << autoIndexPageListing(request.getPath());
        _response = buffer.str();
        _type = "text/html";
    }
    else {
        _statusCode = 404; // not found
        _response = errorHtml(_statusCode);
    }
}


/* GETTERS */
std::string Response::getVersion(void) const {
    return _version;
}

unsigned int Response::getStatusCode(void) const {
    return _statusCode;
}

std::string Response::getResponse(void) const {
    return _response;
}


/* UTILS */
bool Response::fileExists(const std::string& path) {
    struct stat buffer;

    if (stat(path.c_str(), &buffer) == 0)
        return true;
    return false;
}
