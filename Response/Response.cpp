#include "../includes/response.h"

Response::Response(void) {
}

Response::~Response() {

}

Response&	Response::operator=(const Response &cpy) {
	this->_version = cpy.getVersion(); // do i need to write all these functions tho??
	this->_statusCode = cpy.getStatusCode();
    this->_response = cpy._response;
    this->_type = cpy._type;
    this->_port = cpy._port; // NEEDED?
    this->_host = cpy._host; // NEEDED?
    this->_isAutoIndex = cpy._isAutoIndex;

    // add more stuff here no
	return (*this);
}


/* PROCESS RESPONSE */
void Response::giveResponse(parseRequest& request) {
    // _statusCode should inherit from the previous thing right?? or if it gets here we assume all is good already??
    _statusCode = request.getRetVal();
    _type = "";
    _isAutoIndex = false; // WRONG NEEDS TO BE UPDATED BASED ON CONFIG FILE -- lou??
    // _path = ; // get it from config ?? Lou?
    initErrorCodes();
    initMethods();// init method map or something?

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
        _response = buildResponseHeader(request); // OVER HERE RIGHT??
    }

    // SEND END RESULT TO LOU
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
    if (cgiInvolved(request.getPath()) == true) {
        // _response = cgi. ; // check with laura
            // retreive response from cgi handler

    }
    else if (_statusCode == 200)
        readContent(request);
    else
        _response = errorHtml(_statusCode); // but could it be 100/201/204 ?? as its not in the list
    _response = buildResponseHeader(request); // TO WRITE -- ALSO NEED WHEN CGI INVOLVED?
}

void Response::postMethod(parseRequest& request) {
    if (cgiInvolved(request.getPath()) == true) {
        // _response = cgi. ; // check with laura
            // retreive response from cgi handler

    }
    else {
        _statusCode = 204; // no content
        _response = "";
    }
    if (_statusCode == 500)
        _response = errorHtml(_statusCode);
    _response = buildResponseHeader(request); // TO WRITE -- ALSO NEED WHEN CGI INVOLVED?
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
        _response = errorHtml(_statusCode);
    _response = buildResponseHeader(request); // TO WRITE
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

void Response::htmlErrorCodesMap() { // but this includes already the whole header thing
    // _errorCodesHtml[400] = "HTTP/1.1 400 Bad Request\r\n\n"
    // "Content-Type: text/html\r\n\nContent-Length: 151\r\n\r\n "
    // "<!DOCTYPE html><html><head><title>400</title></head><body><h1> 400 Bad Request Error! </h1><p>We are not speaking the same language!</p></body></html>";
    // _errorCodesHtml[403] = "HTTP/1.1 403 Forbiden\r\n\n"
    // "Content-Type: text/html\r\n\nContent-Length: 130\r\n\r\n "
    // "<!DOCTYPE html><html><head><title>403</title></head><body><h1> 403 Forbiden! </h1><p>This is top secret, sorry!</p></body></html>";
    // _errorCodesHtml[404] = "HTTP/1.1 404 Not Found\r\n\n"
    // "Content-Type: text/html\r\n\nContent-Length: 115\r\n\r\n "
    // "<!DOCTYPE html><html><head><title>404</title></head><body><h1> 404 Page not found! </h1><p>Puff!</p></body></html>";
    // _errorCodesHtml[405] = "HTTP/1.1 405 Method Not Allowed\r\n\n"
    // "Content-Type: text/html\r\n\nContent-Length: 139\r\n\r\n "
    // "<!DOCTYPE html><html><head><title>405</title></head><body><h1> 405 Method Not Allowed! </h1><p>We forgot how to do that!</p></body></html>";
    // _errorCodesHtml[413] = "HTTP/1.1 413 Payload Too Large\r\n\n"
    // "Content-Type: text/html\r\n\nContent-Length: 163\r\n\r\n "
    // "<!DOCTYPE html><html><head><title>413</title></head><body><h1> 413 Payload Too Large! </h1><p>We are too busy right now, please try again later!</p></body></html>";
    // _errorCodesHtml[500] = "HTTP/1.1 500 Internal Server Error\r\n\n"
    // "Content-Type: text/html\r\n\nContent-Length: 146\r\n\r\n "
    // "<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";

    _errorCodesHtml[400] = "<!DOCTYPE html><html><head><title>400</title></head><body><h1> 400 Bad Request Error! </h1><p>We are not speaking the same language!</p></body></html>";
    _errorCodesHtml[403] = "<!DOCTYPE html><html><head><title>403</title></head><body><h1> 403 Forbiden! </h1><p>This is top secret, sorry!</p></body></html>";
    _errorCodesHtml[404] = "<!DOCTYPE html><html><head><title>404</title></head><body><h1> 404 Page not found! </h1><p>Puff!</p></body></html>";
    _errorCodesHtml[405] = "<!DOCTYPE html><html><head><title>405</title></head><body><h1> 405 Method Not Allowed! </h1><p>We forgot how to do that!</p></body></html>";
    _errorCodesHtml[413] = "<!DOCTYPE html><html><head><title>413</title></head><body><h1> 413 Payload Too Large! </h1><p>We are too busy right now, please try again later!</p></body></html>";
    _errorCodesHtml[500] = "<!DOCTYPE html><html><head><title>500</title></head><body><h1> 500 Internal Server Error! </h1><p>I probably should study more!</p></body></html>";
    if (_statusCode == 400 || _statusCode == 403 || _statusCode == 404 ||
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

// std::string Response::readHtmlFile(const std::string &path) { // this function needed actually??
//     std::ofstream file; // allows to write to an outfile

//     if (fileExists(path) == true){
//         file.open(path.c_str(), std::ifstream::in); // flag opening it for reading purpose
//         if (!file.is_open())
//             return ("<!DOCTYPE html><body><h1> 404 </h1><p> Page Not Found </p></body></html>");
        
//         std::stringstream buffer;
//         buffer << file.rdbuf();
//         std::string content = buffer.str();
//         file.close();
//         _type = "text/html";
//         return (content);
//     }
//     else
//         return ("<!DOCTYPE html><body><h1> 404 </h1><p> Page Not Found </p></body></html>");
// }

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
        std::stringstream buffer;
        buffer << autoIndexPageListing(request.getPath(), _host, _port); // need to get these from lou tho
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
        return false;
    return true;
}
