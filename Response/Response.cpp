#include "../includes/response.h"

Response::Response(void) : _response(""), _version(""), _statusCode(200), _statusText(""), _respBody("") {
    // _statusCode should inherit from the previous thing right?? or if it gets here we assume all is good already??

    // init method map or something?
    // based on that send to work on the right method
    if (_statusCode != 200)
        std::cout << "BIG PROBLEME\n"; // of courase not, revise this
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


/* METHOD FUNCTIONS */

void Response::getMethod() {

}

void Response::postMethod() {

}

void Response::deleteMethod(parseRequest& request) {
    _response = "";
    if (fileExists(_path) == true){ // see if it was removed or not
        // accordingly set _statusCode
        if (remove(_path.c_str()) == 0)
            _statusCode = 204; // meaning no content, returned to indicate success and there is no body message
        else
            _statusCode = 403; // forbidden to rm, insufficient rights
    }
    else
        _statusCode = 404; // not found
    if (_statusCode == 404 || _statusCode == 403)
        // _response = ; // redirect to write error page or something right?? HTML format
    // _response ; // do something with the header to send
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
bool Response::fileExists(std::string path) {
    struct stat s;

    if (stat(path.c_str(), &s) == 0)
        return false;
    return true;
}