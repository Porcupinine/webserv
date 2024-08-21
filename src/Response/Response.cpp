/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Response.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/07 15:49:40 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/21 16:46:07 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Response.hpp"
#include "../../inc/Server.hpp"

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
std::string Response::giveResponse(ParseRequest& request, struct SharedData &shared) {
    if (request.getPath() == "/favicon.ico") {
        _statusCode = 200;
        _response = "HTTP/1.1 200 OK\r\n"
        "Content-Language: en, en\r\n"
        "Connection: closed\r\n\r\n";
        shared.response = _response;
        return _response;
    }
    
    if (request.getRedirection() == true) {
        std::map<int, std::string> redirMap = shared.server_config->getRedirect(request.getPath());
        if (redirMap.begin()->first == 0)
            _statusCode = request.getRetVal();
        else
            _statusCode = redirMap.begin()->first;
    }
    else
        _statusCode = request.getRetVal();
    _type = "";
    
    _isAutoIndex = shared.server_config->auto_index;
    if (request.getRedirection() == true) {
        _isAutoIndex = shared.server_config->getDirListing(request.getRawPath());
        std::cout << "AM I OVER HERE??? HELOOOO " << _isAutoIndex << "\n"; // to rm
        if (_isAutoIndex == false && (_statusCode != 301 && _statusCode != 302 && _statusCode != 307 && _statusCode != 308))
            _statusCode = 403;
    }
        
    initErrorCodes();
    htmlErrorCodesMap();
    initMethods();
    if (shared.server_config->max_client_body_size < request.getBodyMsg().size())
        _statusCode = 413;
        
    std::map<std::string, ResponseCallback>::iterator it = _method.find(request.getMethod());
    if (it != _method.end())
        (this->*(it->second))(request, &shared);
    else
        _statusCode = 405;
    if (_statusCode == 405 || _statusCode == 413 || _statusCode == 403) {
        _response = errorHtml(_statusCode, &shared, request);
        _response = buildResponseHeader(request, &shared);
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
void Response::getMethod(ParseRequest& request, struct SharedData* shared) {
    if (_statusCode == 200) {
        readContent(request, shared);
        _response = buildResponseHeader(request, shared);
    }
    else if (request.getRedirection() == true) {
        if (_statusCode == 301 || _statusCode == 302 || _statusCode == 307 || _statusCode == 308)
            _response = errorHtml(_statusCode, shared, request);
        else
            readContent(request, shared);
        _response = buildResponseHeader(request, shared);
    }
    else
        _response = errorHtml(_statusCode, shared, request);
}

void Response::postMethod(ParseRequest& request, struct SharedData* shared) {
    if (cgiInvolved(request.getPath()) == false) {
        _statusCode = 204;
        _response = "";
        _response = buildResponseHeader(request, shared);
    }
    if (_statusCode == 500) {
        _response = errorHtml(_statusCode, shared, request);
        _response = buildResponseHeader(request, shared);
    }
}

void Response::deleteMethod(ParseRequest& request, struct SharedData* shared) {
    _response = "";

    if (fileExists(request.getPath()) == true){
        if (remove(request.getPath().c_str()) == 0)
            _statusCode = 204;
        else
            _statusCode = 403;
    }
    else
        _statusCode = 404;

    if (_statusCode == 404 || _statusCode == 403)
        _response = errorHtml(_statusCode, shared, request);
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
    _errorCodes[408] = "Request Timeout";
	_errorCodes[413] = "Payload Too Large";
	_errorCodes[500] = "Internal Server Error";
    _errorCodes[504] = "Gateway Timeout";
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
    _errorCodesHtml[408] = "<!DOCTYPE html><html><head><title>408</title></head><body><h1> 408 Request Timeout! </h1></body></html>";
    _errorCodesHtml[413] = "<!DOCTYPE html><html><head><title>413</title></head><body><h1> 413 Payload Too Large! </h1><p>We are too busy right now, please try again later!</p></body></html>";
    _errorCodesHtml[500] = "<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
    _errorCodesHtml[504] = "<!DOCTYPE html><html><head><title>504</title></head><body><h1> 504 Gateway Timeout! </h1></body></html>";
    if (_statusCode == 301  || _statusCode == 302 || _statusCode == 307 || _statusCode == 308 ||
    _statusCode == 400 || _statusCode == 403 || _statusCode == 404 || _statusCode == 405 ||
    _statusCode == 408 || _statusCode == 413 || _statusCode == 500 || _statusCode == 504)
        _type = "text/html";
}


/* HTML RELATED */
std::string Response::errorHtml(unsigned int error, struct SharedData* shared, ParseRequest& request) {
    std::map<int, std::string>::iterator it = shared->server_config->error_pages.find(error);
    if (it != shared->server_config->error_pages.end()) {
        std::ifstream file(request.getAbsPath() + shared->server_config->root_dir + it->second);
        if (file) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        } else {
            std::cerr << "Failed to open error page file: " << it->second << std::endl;
        }
    }

    std::map<unsigned int, std::string>::iterator it2 = _errorCodesHtml.find(error);
    if (it2 == _errorCodesHtml.end())
        return ("<!DOCTYPE html><body><h1> 404 </h1><p> Error Page Not Found </p></body></html>");
    else
        return (it2->second);
}

void Response::readContent(ParseRequest& request, struct SharedData* shared) {
    std::ifstream file;
    
    if (fileExists(request.getPath()) == true && request.getRawPath() == "") {
        file.open((request.getPath().c_str()), std::ifstream::in);
        if (!file.is_open()) {
            _statusCode = 403;
            _response = errorHtml(_statusCode, shared, request);
            return ;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        _response = content;
        file.close();
    }
    // else if (_isAutoIndex == false && request.getDir() == true && fileExists(request.getPath() + "/index.html")) {
    else if (_isAutoIndex == false && request.getDir() == true && fileExists(request.getPath() + "/" + shared->server_config->getIndex(request.getRawPath()))) {
        // CHEK IF THERE IS ALREADY A BACK SLASH OR NOT IF NOT ADD ELSE TRIM OR SOMETHING TO NOT HAVE A BUG THERE
        
        // std::string f = request.getPath() + "/" + shared->server->getIndex(request.getRawPath()); // put this back when fixed
        std::string f = request.getPath() + "/index.html";
        file.open(f.c_str(), std::ifstream::in);
        if (!file.is_open()) {
            _statusCode = 403;
            _response = errorHtml(_statusCode, shared, request);
            return ;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        _response = content;
        _statusCode = 200;
        file.close();
    }
    else if (_isAutoIndex == true) {
        std::stringstream buffer;
        buffer << autoIndexPageListing(request.getPath(), request.getRawPath());
        _response = buffer.str();
        _type = "text/html";
    }
    else {
        _statusCode = 404;
        _response = errorHtml(_statusCode, shared, request);
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
