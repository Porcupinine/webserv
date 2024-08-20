/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 15:50:05 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/20 15:34:42 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ParseRequest.hpp"
#include "../../inc/Server.hpp"

ParseRequest::ParseRequest::ParseRequest(struct SharedData* shared) : _methodType(""), _path(""), _version(""), _bodyMsg(""), _port(0), _returnValue(200), _query(""), _redirection(false), _dir(false), _rawPath("") {
    initHeaders();
    if (shared->request.empty())
        shared->status = Status::closing;
    parseStr(shared->request, shared);
    if (cgiInvolved(_path) == false) {
        shared->connection_closed = true;
		shared->status = Status::writing;
    }
	std::cout << "response is " << shared->response << "\n"; // to rm
}

ParseRequest&	ParseRequest::operator=(const ParseRequest &cpy)
{
	this->_headers = cpy.getHeaders();
	this->_methodType = cpy.getMethod();
	this->_version = cpy.getVersion();
	this->_returnValue = cpy.getRetVal();
	this->_bodyMsg = cpy.getBodyMsg();
	this->_port = cpy.getPort();
	this->_path = cpy.getPath();
    this->_query = cpy._query;
	this->_absPathRoot = cpy._absPathRoot;
    this->_redirection = cpy.getRedirection();
    this->_rawPath = cpy.getRawPath();
	return (*this);
}

void ParseRequest::parseStr(std::string &info, struct SharedData* shared) {
    if (shared->response_code != 200) {
        errorServer(shared);
        return ;
    }
    
    size_t i = 0;
    std::string line;
    std::string bodyLine;
    std::string value;
    std::string key;

    _port = shared->server_config->port;
    parseFirstline(readLine(info, i), shared);
    while ((line = readLine(info, i)) != "\r" && line != "" && _returnValue != 400) {
        key = setKey(line);
        value = setValue(line);
        if (_headers.count(key)) {
            _headers[key] = value;
        }
    }
	size_t startBody = info.find("\r\n\r\n") + 4;
	_bodyMsg = info.substr(startBody, std::string::npos);
    setPort(_headers["Host"]);
    setQuery();
    setLanguage();

    if (_headers["Cookie"] != "")
        _cookies = parseCookies(_headers["Cookie"]);
    
    _cgiresponse = "";
    if (cgiInvolved(_path) == true) {
        shared->status = Status::start_cgi; // TODO changed the arg
        return ;
    }
	
    Response res;
    shared->response = res.giveResponse(*this, *shared);
    shared->status = Status::writing;
}

void ParseRequest::errorServer(struct SharedData* shared) {
    std::map<int, std::string>::iterator it;
    if (shared->response_code == 408 || shared->response_code == 504)
        it = shared->server_config->error_pages.find(shared->response_code);
    else
        it = shared->server_config->error_pages.find(500);
    if (it != shared->server_config->error_pages.end()) {
        std::string current = "";
        try {
            current = std::filesystem::current_path();
        }
        catch (std::exception &ex) {
            std::cerr << "Error: " << ex.what();
        }
        if (current.find("/build") != std::string::npos) {
            std::size_t found = current.find_last_of("/");
            current.erase(found);
        }
        std::ifstream file(current + shared->server_config->root_dir + it->second);
        if (file) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            size_t len = content.length();
            if (shared->response_code == 408)
                shared->response = "HTTP/1.1 408 Request Timeout\r\n";
            else if (shared->response_code == 504)
                shared->response = "HTTP/1.1 504 Gateway Timeout\r\n";
            else
                shared->response = "HTTP/1.1 500 Internal Server Error\r\n";
            shared->response += "Content-Type: text/html\r\n"
                            "Content-Length: " + std::to_string(len) + 
                            "\r\nConnection: closed\r\n\r\n" + content;
            return ;
        } else {
            std::cerr << "Failed to open error page file: " << it->second << std::endl;
        }
    }
    if (shared->response_code == 408)
        shared->response = "HTTP/1.1 408 Request Timeout\r\n"
        "Content-Type: text/html\r\nContent-Length: 104\r\nConnection: closed\r\n\r\n"
        "<!DOCTYPE html><html><head><title>408</title></head><body><h1> 408 Request Timeout! </h1></body></html>";
    else if (shared->response_code == 504) 
        shared->response = "HTTP/1.1 504 Gateway Timeout\r\n"
        "Content-Type: text/html\r\nContent-Length: 104\r\nConnection: closed\r\n\r\n"
        "<!DOCTYPE html><html><head><title>504</title></head><body><h1> 504 Gateway Timeout! </h1></body></html>";
    else
        shared->response = "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\nContent-Length: 146\r\nConnection: closed\r\n\r\n"
        "<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
}

std::string ParseRequest::readLine(const std::string &str, size_t &i) {
    std::string res;
    size_t j;

    if (i == std::string::npos) 
        return "";
    j = str.find_first_of('\n', i);
    res = str.substr(i, j - i);
    if (res[res.size() - 1] == '\r')
        res.pop_back();
    if (j == std::string::npos)
        i = j;
    else
        i = j + 1;
    return res;
}

/* SETS THE HEADER VALUES */
std::string ParseRequest::setKey(const std::string &line) {
    size_t i;
    std::string res;

    i = line.find_first_of(":", 1);
    res.append(line, 0, i);
    capsOn(res);
    return res;
}

std::string ParseRequest::setValue(const std::string &line) {
    size_t i;
    size_t endline;
    std::string res;

    i = line.find_first_of(":", 1);
    i = line.find_first_not_of(" ", i + 1);
    endline = line.find_first_of("\r", i);
    line.substr(i, endline - 1);
    if (i != std::string::npos)
        res.append(line, i, std::string::npos);
    return rmSpaces(res);
}

void ParseRequest::setLanguage() {
    std::vector<std::string> vec;
    std::string header;
    size_t i;

    if (!(header = _headers["Accept-Language"]).empty())
    {
        vec = split(header, ',');
        for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
        {
            float weight = 0.0;
			std::string	language;
			language = (*it).substr(0, (*it).find_first_of('-'));
			rmSpaces(language);
			if ((i = language.find_last_of(';')) != std::string::npos) {
				weight = atof((*it).substr(i + 4).c_str());
			}
            if (i > 2)
                language.resize(2);
            else
                language.resize(i);
			_language.push_back(std::pair<std::string, float>(language, weight));
		}
        _language.sort(std::greater<std::pair<std::string, float>>());
    }
}

std::string ParseRequest::readBody(const std::string &str, size_t &i) {
    std::string res;

    if (i == std::string::npos || i >= str.size())
        return "";
    for (; i < str.size(); i++) {
        res += str[i];
    }
    return res;
}

void ParseRequest::setQuery() {
    size_t i;

    i = _path.find_first_of('?');
    if (i != std::string::npos) {
        _query.assign(_path, i + 1, std::string::npos);
        _path = _path.substr(0, i);
    }
}

/* ACCESSSORS SETTERS-GETTERS */
void ParseRequest::setMethod(std::string type) {
    _methodType = type;
}

std::string ParseRequest::getMethod(void) const {
    return _methodType;
}

void ParseRequest::setPath(std::string path) {
    _path = path;
}

std::string ParseRequest::getPath(void) const {
    return _path;
}

void ParseRequest::setVersion(std::string v) {
    _version = v;
}

std::string ParseRequest::getVersion(void) const {
    return _version;
}

void ParseRequest::setPort(std::string port) {
    size_t start;

    start = port.find_first_of(":");

    if (start != 0 && port.find("localhost:") != std::string::npos)
        port = port.substr(start + 1);
    if (port.size() < 5 && _returnValue == 200){
        try {
            _port = std::stoul(port);
        } catch (const std::out_of_range& e) {
            std::cerr << "Error: Port number out of range "<<_port<< "\n";
        }
    }
    else
        std::cerr << "Error: in port\n";
}

unsigned int ParseRequest::getPort(void) const {
    return _port;
}

void ParseRequest::setRetVal(int value) {
    _returnValue = value;
}

int ParseRequest::getRetVal(void) const {
    return _returnValue;
}

void ParseRequest::setBodyMsg(std::string b) {
	_bodyMsg = b;
}

std::string ParseRequest::getBodyMsg(void) const {
    return _bodyMsg;
}

std::string ParseRequest::getLanguageStr(void) const {
    std::ostringstream oss;

    for (auto it = _language.begin(); it != _language.end(); ++it) {
        oss << it->first;
        if (std::next(it) != _language.end()) {
            oss << ", ";
        }
    }
    return oss.str();
}

std::string ParseRequest::getQuery(void) const {
    return _query;
}

std::string ParseRequest::getCgiResponse(void) const {
    return _cgiresponse;
}

std::string ParseRequest::getRawPath(void) const {
    return _rawPath;
}

bool ParseRequest::getRedirection(void) const {
	return _redirection;
}

bool ParseRequest::getDir(void) const {
	return _dir;
}

/* HEADERS */
void ParseRequest::initHeaders() {
    _headers.clear();
    _headers["Accept-Charsets"] = "";
    _headers["Accept-Language"] = "";
    _headers["Allow"] = "";
	_headers["Auth-Scheme"] = "";
	_headers["Authorization"] = "";
	_headers["Connection"] = "Keep-Alive";
    _headers["Cookie"] = "";
	_headers["Content-Language"] = "";
	_headers["Content-Length"] = "";
	_headers["Content-Location"] = "";
	_headers["Content-Type"] = "";
	_headers["Date"] = "";
	_headers["Host"] = "";
	_headers["Last-Modified"] = "";
	_headers["Location"] = "";
	_headers["Referer"] = "";
	_headers["Retry-After"] = "";
	_headers["Server"] = "";
	_headers["Transfer-Encoding"] = "";
	_headers["User-Agent"] = "";
	_headers["Www-Authenticate"] = "";
}

const std::map<std::string, std::string>&	ParseRequest::getHeaders(void) const {
	return _headers;
}

const std::map<std::string, std::string>&	ParseRequest::getCookies(void) const {
	return _cookies;
}

/* PARSING REQUEST */
int ParseRequest::parseFirstline(const std::string &info, struct SharedData* shared) {
    size_t i;
    std::string line;

    try {
        i = info.find_first_of('\n');
        line = info.substr(0, i);
    } catch (const std::out_of_range& e) {
        _returnValue = 400;
        std::cerr << "Error: substring out of range: " << e.what() << "\n";
        return _returnValue;
    }

    i = line.find_first_of(' ');
    
    if (i == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: wrong syntax of HTTP method\n";
        return _returnValue;
    }
    _methodType.assign(line, 0, i);
    return parsePath(line, i, *shared);
}

int ParseRequest::parsePath(const std::string &line, size_t i, struct SharedData &shared) {
    size_t j;

    if ((j = line.find_first_not_of(' ', i)) == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: no path\n";
        return _returnValue;
    }
    if ((j = line.find_first_of(' ', j)) == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: no HTTP version\n";
        return _returnValue;
    }

    _path.assign(line, i + 1, j - i);
    trim(_path);
	
    if (_path == "/favicon.ico")
		return _returnValue;

    Locations *loc = shared.server->getLocation(_path);

    std::string abspath = shared.server->getRootFolder(_path);
    std::string current = "";
	try {
		current = std::filesystem::current_path();
	}
	catch (std::exception &ex) {
		std::cerr << "Error: " << ex.what();
        _returnValue = 500;
        return 500;
	}
    if (current.find("/build") != std::string::npos) {
        std::size_t found = current.find_last_of("/");
        current.erase(found);
    }

    _absPathRoot = current;

    Locations *spe = shared.server->getSpecifier(_path);
    if (spe != nullptr) {
        if (spe->root_dir != ""){
            _dir = true;
            _redirection = true;
            size_t pos = 0;
            while ((pos = _path.find(spe->specifier, pos)) != std::string::npos) {
                _path.erase(pos, spe->specifier.length());
            }
            _path = _absPathRoot + spe->root_dir + _path;
            _rawPath = spe->specifier;
        }
    }

	if ((_path[0] == '/' && _path.size() == 2) || _path == "/")
        _path = _absPathRoot + abspath + "/" + shared.server->getIndex(_path);
    else if (loc != nullptr) {
		if (loc->specifier == _path)
			_redirection = true;
        std::map<int, std::string> redirMap2 = shared.server->getRedirect(_path);
		if (loc->specifier == _path && redirMap2.begin()->first == 0 && _dir == false){
            _dir = true;
            _rawPath = _path;
		    _path = _absPathRoot + abspath + _path;
        }
    }
    else if (cgiInvolved(_path) == true) {
		_path = current + _path;
    }
    else if (_dir == false && !isFileExists(_path)){
        _path = _absPathRoot + abspath + _path;
    }
    // std::cout << "PATH HERE= " << _path << " ABS= " << _absPathRoot << " RAW PATH = " << _rawPath <<"\n"; // to rm
    return parseVersion(line, j, shared);
}

int ParseRequest::parseVersion(const std::string &line, size_t i, struct SharedData &shared) {
    if ((i = line.find_first_not_of(' ', i)) == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: no HTTP version\n";
        return _returnValue;
    }
    if (line[i] == 'H' && line[i + 1] == 'T' && line[i + 2] == 'T' && line[i + 3] == 'P'
        && line[i + 4] == '/')
        _version.assign(line, i  + 5, 3);
    if (_version != "1.0" && _version != "1.1") {
        _returnValue = 400;
        std::cerr << "Error: wrong HTTP version\n";
        return _returnValue;
    }
    return validateMethodType(shared);
}

/* METHOD */
std::string initMethodString(Method method)
{
    switch(method)
    {
        case Method::GET:
            return "GET";
        case Method::POST:
            return "POST";
        case Method::DELETE:
            return "DELETE";
    }
	return "DONE INITING METHOD STRING";
}

int ParseRequest::validateMethodType(struct SharedData &shared) {
    if (_redirection == true) {
        std::set<std::string> allowedMethods = shared.server->getAllowedMethods(_rawPath);
        if (allowedMethods.find(_methodType) == allowedMethods.end())
            _returnValue = 405;
        return _returnValue;
    }
    else if (_methodType == initMethodString(Method::GET) 
    || _methodType == initMethodString(Method::POST) 
    || _methodType == initMethodString(Method::DELETE))
          return _returnValue;
    std::cerr << "Error: invalid method requested\n";
    _returnValue = 400;
    return _returnValue;
}
