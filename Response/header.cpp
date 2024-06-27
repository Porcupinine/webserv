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
	_contentLocation = "";
	_contentType = "";
	_date = "";
	// _lastModified = "";
	// _location = "";
	// _retryAfter = ""; // needed??
	_server = "";
	// _transferEncoding = ""; // needed??
}

/* HEADER SETTERS */
void Response::setHeaderValues(parseRequest& request) {
	_allow = setAllow(request);
	_contentLanguage = request.getLanguageStr();
	_contentLength = _response.size(); // test if this needs to be converted to string
	_contentLocation = request.getPath(); // could this have chnaged based on config file??
	_contentType = _type; // but what if empty??
	_date = setDate(request); // i am putting the date of response not request is that fine??
	// _lastModified = ""; // check where to get this info
	// _location = ""; // 
	// _retryAfter = ""; // needed??
	_server = ""; // needs to come from Lou
	// _transferEncoding = ""; // needed??
	// do we need to add connection (as closed)??
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
	if (_contentLocation != "")
		header += "Content-Location: " + _contentLocation + "/r/n";
	if (_date != "")
		header += "Date: " + _date + "/r/n";
	
	// FIX THE SETTERS FOR THESE FIRST
	// if (_lastModified != "")
	// 	header += "Last-Modified: " + _lastModified + "/r/n";
	// if (_location != "")
	// 	header += "Location: " + _location + "/r/n";
	// if (_retryAfter != "")
	// 	header += "Retry-After: " + _retryAfter + "/r/n";
	if (_server != "")
		header += "Server: " + _server + "/r/n"; // FROM LOU??
	// if (_transferEncoding != "")
	// 	header += "Transfer-Encoding: " + _transferEncoding + "/r/n";
	// if (_connection != "")
	// 	header += "Connection: " + _connection + "/r/n"; // needed??

	if (_response != "")
		header += "/r/n" + _response + "/r/n"; // is this right?? no clue
	return header;
}

std::string Response::getMatchingCodeString(unsigned int code) {
    std::map<unsigned int, std::string>::iterator it = _errorCodes.find(code);

    if (it == _errorCodes.end())
        return (""); // or something else??
    else
        return (it->second);
}