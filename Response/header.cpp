#include "../includes/response.h"

std::string Response::buildResponseHeader(parseRequest& request) {
    // _response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(), requestConf.getLang()) + "\r\n" + _response;
		std::string header = "";
		// WRITE THAT FIRST LINE GET HTTP/1.1 200 OK -- OR SOMETHING

		initResponseHeaderFields();
		setHeaderValues(request);
		// header += getHeaderValues(request, header); // write this
}


void Response::initResponseHeaderFields() {
	_allow = "";
	_contentLanguage = "";
	_contentLength = "";
	_contentLocation = "";
	_contentType = "";
	_date = "";
	_lastModified = "";
	_location = "";
	_retryAfter = ""; // needed??
	_server = "";
	_transferEncoding = ""; // needed??
}

/* HEADER SETTERS */
void Response::setHeaderValues(parseRequest& request) {
	_allow = setAllow(request);
	_contentLanguage = request.getLanguageStr();
	_contentLength = ""; // how to find this out??
	_contentLocation = request.getPath(); // could this have chnaged based on config file??
	_contentType = _type; // but what if empty??
	_date = setDate(request); // i am putting the date of response not request is that fine??
	_lastModified = "";
	_location = "";
	_retryAfter = ""; // needed??
	_server = "";
	_transferEncoding = ""; // needed??
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
		header += _allow + "/r/n";
	
	// DO FOR ALL THE OTHERS

}

std::string Response::getMatchingCodeString(unsigned int code) {
    std::map<unsigned int, std::string>::iterator it = _errorCodes.find(code);

    if (it == _errorCodes.end())
        return (""); // or something else??
    else
        return (it->second);
}