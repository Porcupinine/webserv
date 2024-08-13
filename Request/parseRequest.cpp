/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parseRequest.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: dmaessen <dmaessen@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/07 15:50:05 by dmaessen      #+#    #+#                 */
/*   Updated: 2024/08/09 19:07:57 by ewehl         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "parseRequest.hpp"

parseRequest::parseRequest(struct SharedData* shared) :  _methodType(""), _version(""), _returnValue(shared->response_code),
                              _bodyMsg(""), _port(shared->server_config->port), _path(""), _query("") {
    initHeaders();
	// std::cout << "req is " << shared->request << "\n";
    if (shared->request.empty())
        shared->status = Status::closing;
//    std::cout << GREEN << "ServerConfig = " << shared->server_config->host << RESET << std::endl; // something up here.
//    std::cout << GREEN << "ServerConfig = " << shared->server_config->root_dir << RESET << std::endl; // something up here.
    parseStr(shared->request, shared);
    if (cgiInvolved(_path) == false)
        shared->status = Status::writing;
	std::cout << "response is " << shared->response << "\n";
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
	return (*this);
}

void parseRequest::parseStr(std::string &info, struct SharedData* shared) {
    if (shared->response_code != 200) { // OR 0??
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

    std::cout << RED << " Nope 1" << RESET << std::endl;
    setPort(_headers["Host"]); // Hier de Fuck#2's TODOMI - Should be fixed.
    std::cout << RED << " Nope 2" << RESET << std::endl;
    setQuery();
    setLanguage();

    if (_headers["Cookie"] != "")
        _cookies = parseCookies(_headers["Cookie"]);
    
    _cgiresponse = "";
    if (cgiInvolved(_path) == true) {
        shared->status = Status::in_cgi; //TODO changed the arg
        std::cout << "going in cgi??\n";
        return ;
    }
    Response res;
    shared->response = res.giveResponse(*this, shared);
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

    if (i == std::string::npos)
        return "";
    for (size_t j = 0; str[i] != std::string::npos; i++) {
        res[j] = str[i];
        j++;
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
	//TODO set different path for redirection and for cgi??
    _path.assign(line, i + 1, j - i);
    std::string abspath = shared.server_config->root_dir;
    std::string current = std::filesystem::current_path();
    std::size_t found = current.find_last_of("/");
    current.erase(found); // to rm after testing as the dir will be fine
//    abspath.erase(0, 1); // this will always be true //TODO it doesn't make sense to keep spreading the dot so both domi and me need to remove it
    _absPathRoot = current;
	if (_path[0] == '/' && _path.size() == 2) {
//		_path = shared.server_config->root_dir + "/htmls/upload.html";
        _path = _absPathRoot + abspath + "/htmls/form.html"; // TODO LOOK INTO THIS -- SHOULD BE index.html BUT FOR NOW TO TEST OTHER PAGES
    }
    else {
		_path = current + _path; //TODO This is fot cgi, for redirect will be different
    }
    std::cout << "PATH HERE= " << _path << " ABS= " << _absPathRoot << "\n"; // to rm
    return parseVersion(line, j);
}

int parseRequest::parseVersion(const std::string &line, size_t i) {
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
    return validateMethodType();
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

int parseRequest::validateMethodType() {
    if (_methodType == initMethodString(Method::GET) 
    || _methodType == initMethodString(Method::POST) 
    || _methodType == initMethodString(Method::DELETE))
          return _returnValue;
    std::cerr << "Error: invalid method requested\n";
    _returnValue = 400;
    return _returnValue;
}
