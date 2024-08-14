/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parseRequest.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/07 15:50:05 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/14 15:45:32 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "parseRequest.hpp"
#include "Server.hpp"

parseRequest::parseRequest::parseRequest(struct SharedData* shared) : _methodType(""), _path(""), _version(""), _bodyMsg(""), _port(0), _returnValue(200), _query(""), _redirection(false), _rawPath("") {
    initHeaders();
    if (shared->request.empty())
        shared->status = Status::closing;
    parseStr(shared->request, shared);
    if (cgiInvolved(_path) == false)
		shared->status = Status::writing;
	std::cout << "response is " << shared->response << "\n"; // to rm
}

//parseRequest::parseRequest() {
//
//}
//
//parseRequest::~parseRequest() {
//
//}

parseRequest&	parseRequest::operator=(const parseRequest &cpy)
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

void parseRequest::parseStr(std::string &info, struct SharedData* shared) {
    if (shared->response_code != 200) { // but check if there is a dedicated page first
        shared->response = "HTTP/1.1 500 Internal Server Error\r\n\n"
        "Content-Type: text/html\r\n\nContent-Length: 146\r\n\r\n "
        "<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
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
        std::cout << "going in cgi??\n";
        return ;
    }
	
    Response res;
    shared->response = res.giveResponse(*this, *shared);
    shared->status = Status::writing;
}


std::string parseRequest::readLine(const std::string &str, size_t &i) {
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
std::string parseRequest::setKey(const std::string &line) {
    size_t i;
    std::string res;

    i = line.find_first_of(":", 1);
    res.append(line, 0, i);
    capsOn(res);
    return res;
}

std::string parseRequest::setValue(const std::string &line) {
    size_t i;
    size_t endline;
    std::string res;

    i = line.find_first_of(":", 1);
    i = line.find_first_not_of(" ", i + 1);
    endline = line.find_first_of("\r", i);
    line.substr(i, endline - 1); //TODO ignoring return value
    if (i != std::string::npos)
        res.append(line, i, std::string::npos);
    return rmSpaces(res);
}

void parseRequest::setLanguage() {
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

std::string parseRequest::readBody(const std::string &str, size_t &i) {
    std::string res;

    if (i == std::string::npos || i >= str.size())
        return "";
    for (; i < str.size(); i++) {
        res += str[i];
    }
    return res;
}

void parseRequest::setQuery() {
    size_t i;

    i = _path.find_first_of('?');
    if (i != std::string::npos) {
        _query.assign(_path, i + 1, std::string::npos);
        _path = _path.substr(0, i);
    }
}

/* ACCESSSORS SETTERS-GETTERS */
void parseRequest::setMethod(std::string type) {
    _methodType = type;
}

std::string parseRequest::getMethod(void) const {
    return _methodType;
}

void parseRequest::setPath(std::string path) {
    _path = path;
}

std::string parseRequest::getPath(void) const {
    return _path;
}

void parseRequest::setVersion(std::string v) {
    _version = v;
}

std::string parseRequest::getVersion(void) const {
    return _version;
}

void parseRequest::setPort(std::string port) {
    size_t start;

    start = port.find_first_of(":");

    if (start != 0 && port.find("localhost:") != std::string::npos)
        port = port.substr(start + 1);
    if (port.size() < 5)
        _port = std::stoul(port);
    else
        std::cerr << "Error: in port\n"; // this will be checked later as well right
}

unsigned int parseRequest::getPort(void) const {
    return _port;
}

void parseRequest::setRetVal(int value) {
    _returnValue = value;
}

int parseRequest::getRetVal(void) const {
    return _returnValue;
}

void parseRequest::setBodyMsg(std::string b) {
	_bodyMsg = b;
}

std::string parseRequest::getBodyMsg(void) const {
    return _bodyMsg;
}

std::string parseRequest::getLanguageStr(void) const {
    std::ostringstream oss;

    for (auto it = _language.begin(); it != _language.end(); ++it) {
        oss << it->first;
        if (std::next(it) != _language.end()) {
            oss << ", ";
        }
    }
    return oss.str();
}

std::string parseRequest::getQuery(void) const {
    return _query;
}

std::string parseRequest::getCgiResponse(void) const {
    return _cgiresponse;
}

std::string parseRequest::getRawPath(void) const {
    return _rawPath;
}

bool parseRequest::getRedirection(void) const {
	return _redirection;
}

/* HEADERS */
void parseRequest::initHeaders() {
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

const std::map<std::string, std::string>&	parseRequest::getHeaders(void) const {
	return _headers;
}

const std::map<std::string, std::string>&	parseRequest::getCookies(void) const {
	return _cookies;
}

/* PARSING REQUEST */
int parseRequest::parseFirstline(const std::string &info, struct SharedData* shared) {
    size_t i;
    std::string line;

    i = info.find_first_of('\n');
    line = info.substr(0, i);
    i = line.find_first_of(' ');
    
    if (i == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: wrong syntax of HTTP method\n"; // OR WHAT??
        return _returnValue;
    }
    _methodType.assign(line, 0, i);
    return parsePath(line, i, *shared);
}

int parseRequest::parsePath(const std::string &line, size_t i, struct SharedData &shared) {
    size_t j;

    if ((j = line.find_first_not_of(' ', i)) == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: no path\n"; // do we want this here, should go further not?
        return _returnValue;
    }
    if ((j = line.find_first_of(' ', j)) == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: no HTTP version\n"; // do we want this here, should go further not?
        return _returnValue;
    }

    _path.assign(line, i + 1, j - i);
    trim(_path);
	
    if (_path == "/favicon.ico")
		return _returnValue; //What is return value here?

    std::cout << GREEN << "getting here" RESET << std::endl; // to rm
    Locations *loc = shared.server->getLocation(_path);

    std::string abspath = shared.server_config->root_dir;
    std::string current = "";
	try {
		current = std::filesystem::current_path(); // this can throw an error, if does, server crashes.
	}
	catch (std::exception &ex) {
		std::cerr << "Errorssss: " << ex.what();
        return (_returnValue = 500); // or something?
	}

    if (current.find("/build") != std::string::npos) {
        std::size_t found = current.find_last_of("/");
        current.erase(found);
    }

    _absPathRoot = current;
	if ((_path[0] == '/' && _path.size() == 2) || _path == "/") {
        _path = _absPathRoot + abspath + "/" + shared.server_config->index;
    }
    else if (loc != nullptr) { // Wot?
		if (loc->specifier == _path)
			_redirection = true; // Wot?
        std::map<int, std::string> redirMap = shared.server->getRedirect(_path);
		if (loc->specifier == _path && redirMap.begin()->first == 0) // or else if?? loc->spec has to be _path, so redundant
		    _rawPath = _path;
            _path = _absPathRoot + abspath + _path;
    }
    else if (cgiInvolved(_path) == true) {
		_path = current + _path;
    }
    else {
        _path = _absPathRoot + abspath + _path;
    }
    std::cout << "PATH HERE= " << _path << " ABS= " << _absPathRoot << "\n"; // to rm
    return parseVersion(line, j, shared);
}

int parseRequest::parseVersion(const std::string &line, size_t i, struct SharedData &shared) {
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

int parseRequest::validateMethodType(struct SharedData &shared) {
    if (_redirection == true) {
        std::set<std::string> allowedMethods = shared.server->getAllowedMethods(_path);
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
