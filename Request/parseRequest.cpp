#include "../Request/parseRequest.hpp"

parseRequest::parseRequest(std::string &info) : _methodType(""), _version(""), _returnValue(200),
                              _bodyMsg(""), _port(80), _path(""), _query(""), _infoStr(info) {
    initHeaders();
    parseStr(info);
    if (_returnValue != 200)
        std::cout << "Parse error: " << _returnValue << '\n';
}

parseRequest::~parseRequest() {
    // SOMETHING TO DESTROY/CLEAN ??
}

parseRequest&	parseRequest::operator=(const parseRequest &cpy)
{
	this->_headers = cpy.getHeaders();
	this->_methodType = cpy.getMethod();
	this->_version = cpy.getVersion();
	this->_returnValue = cpy.getRetVal();
	this->_bodyMsg = cpy.getBodyMsg();
	this->_port = cpy.getPort();
	this->_path = cpy.getPath();
    // NOT QUERY??
	return (*this);
}

void parseRequest::parseStr(std::string &info) {
    size_t i = 0;
    std::string line;
    std::string value;
    std::string key;

    parseFirstline(readLine(info, i)); // 400 RET?? -- BUT NEED TO HAVE A STACK RIGHT TO PUT ALL THIS STUFF ON
    // CHECK IF IT RETURNS 400 OR NOT -- AS ITS A BAD REQUEST
    while ((line = readLine(info, i)) != "\r" && line != "" && _returnValue != 400) {
        key = setKey(line); // identifier of header
        value = setValue(line);
        if (_headers.count(key))
            _headers[key] = value;
        // if (line == "\n") {
        //     line = readBody(info, i); // or does i need to be +1 here? if it hangs on the newline still
        //     setBody(line);
        //     break ;
        // }
    }
    setQuery(); // then to decode later right?? or something
    setLanguage();
    setBody(); // if any as they body comes after the headers and a newline first THEN the body message
    //return _returnValue; // DEPENDS IF VOID OR INT TO BE RETURNED
}
//TODO can't compile as the functions don't take any argument

std::string parseRequest::readLine(const std::string &str, size_t &i) {
    std::string res;
    size_t j;

    if (i == std::string::npos)
        return ""; // empty sring
    j = str.find_first_of('\n', 1);
    res = str.substr(i, j - i);
    if (res[res.size() - 1] == '\r')
        res.pop_back(); // rm last char if \r
    if (j == std::string::npos) // if we'r at the end
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
    capsOn(res); // maybe this can go in a certain utils or something 
    return res;
}

std::string parseRequest::setValue(const std::string &line) {
    size_t i;
    size_t len;
    std::string res;

    i = line.find_first_of(":", 1);
    i = line.find_first_not_of(" ", i + 1); // so the search begings after :
    if (i != std::string::npos)
        res.append(line, i, std::string::npos);
    len = res.size();
    return rmSpaces(res);
}

void parseRequest::setLanguage() {
    std::vector<std::string> vec;
    std::string header;
    size_t i;

    if ((header = _headers["Accept-Language"]) != "")
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
        _language.sort(std::greater<std::pair<std::string, float>>()); // having the biggest on top
    }
}

std::string parseRequest::readBody(const std::string &str, size_t &i) {
    std::string res;

    if (i == std::string::npos)
        return "";
    for (size_t j = 0; str[i] != std::string::npos; i++) { // EOF or std::string::npos
        res[j] = str[i];
        j++;
    }
    return res;
}

void	parseRequest::setBody(const std::string &str) {
    _bodyMsg.assign(str);

	for (int i = _bodyMsg.size() - 1; i >= 0; --i) {
		if (_bodyMsg[i] == '\n' || _bodyMsg[i] == '\r')
			_bodyMsg.erase(i);
		else
			break ;
    }
}

void parseRequest::setQuery() {
    size_t i;

    i = _path.find_first_of('?');
    if (i != std::string::npos) {
        _query.assign(_path, i + 1, std::string::npos);
        _path = _path.substr(0, i); // why do we actually strip the _path ??
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
    port = port.std::string::substr(port.find("localhost:"));
    // std::cout << "after trim: " << port << '\n'; // to rm
    if (port.size() < 5)
        _port = std::stoul(port);
    else
        std::cout << "ERROR IN PORT\n"; // this will be checked later as well right
    // std::cout << "after conversion: " << port << '\n'; // to rm
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

/* HEADERS */

void parseRequest::initHeaders() { // DO WE REALLY NEED ALL OF THEM??
    _headers.clear();
    _headers["Accept-Charsets"] = "";
    _headers["Accept-Language"] = "";
    _headers["Allow"] = "";
	_headers["Auth-Scheme"] = "";
	_headers["Authorization"] = ""; // 
	_headers["Connection"] = "Keep-Alive";
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
    // If-Modified-Since ??
}

const std::map<std::string, std::string>&	parseRequest::getHeaders(void) const {
	return _headers;
}


/* PARSING REQUEST */

int parseRequest::parseFirstline(const std::string &info) {
    size_t i;
    std::string line;

    i = info.find_first_of('\n'); // pour avoir la premiere ligne
    line = info.substr(0, i); // isole premiere ligne
    i = line.find_first_of(' ');

    if (i == std::string::npos) {
        _returnValue = 400;
        std::cerr << "Error: wrong syntax of HTTP method\n"; // OR WHAT??
        return _returnValue;
    }
    _methodType.assign(line, 0, i);
    return parsePath(line, i);
}

int parseRequest::parsePath(const std::string &line, size_t i) {
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
    _path.assign(line, j, i - j);
    return parseVersion(line, i);
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
	return "HEY";
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
