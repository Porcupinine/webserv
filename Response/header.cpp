#include "../includes/response.h"

std::string Response::buildResponseHeader(parseRequest& request) {
    std::string header = "";
	initResponseHeaderFields();
	setHeaderValues(request);
	header = getHeaderValues(request, header); // FINALISE THIS
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
	_contentLength = _response.size(); // test if this needs to be converted to string
	_contentType = _type; // but what if empty??
	_date = setDate(request);
	_location = ""; // only use in case redirection 301/302/307/308 -- get from Lou
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
std::string Response::getHeaderValues(parseRequest& request, std::string header) {
	header += request.getVersion() + " " + std::to_string(_statusCode) + " " + getMatchingCodeString(_statusCode) + "/r/n"; // first line
	if (_allow != "")
		header += "Allow: " + _allow + "/r/n";
	if (_contentLanguage != "")
		header += "Content-Language: " + _contentLanguage + "/r/n";
	if (_contentLength != "")
		header += "Content-Length: " + _contentLength + "/r/n";
	if (_date != "")
		header += "Date: " + _date + "/r/n";
	if (_statusCode == 301 || _statusCode == 302 || _statusCode == 307 || _statusCode == 308)
		header += "Location: " + _location + "/r/n"; // GET FROM CONFIG FILE
	header += "Connection: closed/r/n"; // am i the one supposed to wrtie this or lou??

	if (_response != "")
		header += "/r/n" + _response + "/r/n"; // is this right?? no clue
	return header;
}

std::string Response::getMatchingCodeString(unsigned int code) {
    std::map<unsigned int, std::string>::iterator it = _errorCodes.find(code);

    if (it == _errorCodes.end())
        return (""); // or something else?? OR 500
    else
        return (it->second);
}

server {
	...-location {
		return 301 google.com;
	}
}