#include "../includes/response.h"

Response::Response(void) {
}

Response::~Response() {

}

Response&	Response::operator=(const Response &cpy) {
	this->_version = cpy.getVersion(); // do i need to write all these functions tho??
	this->_statusCode = cpy.getStatusCode();
	this->_statusText = cpy.getStatusText();
    this->_respBody = cpy.getRespBody();
	return (*this);
}


/* PROCESS RESPONSE */
void Response::giveResponse(parseRequest& request) {
    // _statusCode should inherit from the previous thing right?? or if it gets here we assume all is good already??
    _statusCode = request.getRetVal();
    _isAutoIndex = false; // WRONG NEEDS TO BE UPDATED BASED ON CONFIG FILE
    initErrorCodes();
    initMethods();// init method map or something?
    // _path = ; // get it from config ??

    // do check with regards to return code 
        // have default responses with error pages (content type HTML, error code, error page)
        // make a map or pointer fucntions for the error pages// based on that send to work on the right method
    if (_statusCode != 200 || _statusCode != 204) // en gros si ca commence pas par 2
        std::cout << "BIG PROBLEME\n"; // of courase not, revise this


    std::map<std::string, void (Response::*)(parseRequest&)>::iterator it = _method.find(request.getMethod());
    if (it != _method.end()) {
        (this->*(it->second))(request);
    }
    else {
        _statusCode = 405;
        _response = errorHtml(_statusCode);
    }
}

/* STATIC INIT */
std::map<std::string, void (Response::*)(parseRequest&)> Response::initMethods()
{
	std::map<std::string, void (Response::*)(parseRequest&)> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;
	return map;
}

std::map<std::string, void (Response::*)(parseRequest &)> Response::_method = Response::initMethods();


/* METHOD FUNCTIONS */
void Response::getMethod(parseRequest& request) {
    // stuff with CGI involved
    if (cgiInvolved(request.getPath()) == true) {// or can i just use the path from repsonse?? 
        // _response = cgi. ; // check with laura
            // retreive resonse from cgi handler
            // have i = start // j - size of cgi response - 2
            // look for the end or for the /r/n/r/n sequence (while loop)
                // then read line by line looking for /r/n
                // if finds "Status:" --> atoi the code and save it as _statusCode
                // if finds "Content-Type: " --> substr in _type
            // then _reponse = _response.substr(i, i - i); // or something like that

    }
    else if (_statusCode == 200)
        readContent(request); // void or int
    else
        _response = errorHtml(_statusCode);
    _response = buildResponseHeader(); // TO WRITE
}

void Response::postMethod(parseRequest& request) {
    // stuff with CGI involved
    if (cgiInvolved(request.getPath()) == true) {// or can i just use the path from repsonse?? 
        // _response = cgi. ; // check with laura
            // retreive resonse from cgi handler
            // have i = start // j - size of cgi response - 2
            // look for the end or for the /r/n/r/n sequence (while loop)
                // then read line by line looking for /r/n
                // if finds "Status:" --> atoi the code and save it as _statusCode
                // if finds "Content-Type: " --> substr in _type
            // then _reponse = _response.substr(i, i - i); // or something like that

    }
    else {
        _statusCode = 204; // no content
        _response = "";
    }
    if (_statusCode == 500)
        _response = errorHtml(_statusCode);
    _response = buildResponseHeader(); // to write
}

void Response::deleteMethod(parseRequest& request) {
    // think this is correct, as CGI/Laura rm it and so i just need to check its really removed
    (void)request; // as we actually don't really need what came out of this
    _response = ""; // as its not initalised

    if (fileExists(request.getPath()) == true){ // see if rm or not
        if (remove(request.getPath().c_str()) == 0)
            _statusCode = 204; // meaning no content, returned to indicate success and there is no body message
        else
            _statusCode = 403; // forbidden to rm, insufficient rights
    }
    else
        _statusCode = 404; // not found

    if (_statusCode == 404 || _statusCode == 403)
        _response = errorHtml(_statusCode); // redirect to write error page or something right?? HTML format
    _response = buildResponseHeader(); // TO WRITE
}

void Response::initErrorCodes()
{
	_errorCodes[100] = "Continue";
	_errorCodes[200] = "OK";
	_errorCodes[201] = "Created";
	_errorCodes[204] = "No Content";
	_errorCodes[400] = "Bad Request";
	_errorCodes[403] = "Forbidden";
	_errorCodes[404] = "Not Found";
	_errorCodes[405] = "Method Not Allowed";
	_errorCodes[413] = "Payload Too Large";
	_errorCodes[500] = "Internal Server Error";
}

std::string Response::buildResponseHeader() {
    
}


/* HTML RELATED */
std::string Response::errorHtml(unsigned int error) {
    // change this based on Laura's map: error code + html string directly
    std::map<unsigned int, std::string>::iterator it = _errorCodes.find(error); // CHANGED THIS

    if (it == _errorCodes.end()) // CHANGE IT HERE TOO
        return ("<!DOCTYPE html><body><h1> 404 </h1><p> Error Page Not Found </p></body></html>");
    else
        return (it->second);
}

std::string Response::readHtmlFile(const std::string &path) { // this function needed actually??
    std::ofstream file; // allows to write to an outfile

    if (fileExists(path) == true){
        file.open(path.c_str(), std::ifstream::in); // flag opening it for reading purpose
        if (!file.is_open())
            return ("<!DOCTYPE html><body><h1> 404 </h1><p> Page Not Found </p></body></html>");
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        _type = "text/html";
        return (content);
    }
    else
        return ("<!DOCTYPE html><body><h1> 404 </h1><p> Page Not Found </p></body></html>");
}

void Response::readContent(parseRequest& request) { // maybe use the above for it adding the autoIndex
    std::ifstream file; // reading content from an infile

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
    else if (_isAutoIndex == true) { // but this needs to be set to true somewhere
        // FIGURE OUT WHAT NEEDS TO BE DONE
        std::stringstream buffer;
        buffer << autoIndexFile(request.getPath(), _host, _port); // need to get these from lou tho
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

std::string Response::getStatusText(void) const {
    return _statusText;
}

std::string Response::getRespBody(void) const {
    return _respBody;
}

std::string Response::getResponse(void) const {
    return _response;
}


/* UTILS */
bool Response::fileExists(const std::string& path) {
    struct stat buffer;

    if (stat(path.c_str(), &buffer) == 0)
        return false;
    return true;
}
