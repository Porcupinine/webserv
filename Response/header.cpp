#include "../includes/response.h"

std::string Response::buildResponseHeader(parseRequest& request) {
    // _response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(), requestConf.getLang()) + "\r\n" + _response;

		initResponseHeaderFields();
		setValues(request);
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

void Response::setValues(parseRequest& request) {
	_allow = setAllow(request);
	_contentLanguage = request.getHeaders("Accept-Language"); // should this be equal to the request header? little confused here
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

std::string Response::setAllow(parseRequest& request) {
	std::ostringstream allowedMethods;

    for (auto it = _method.begin(); it != _method.end(); it++) {
        allowedMethods << it->first;
        if (it != _method.end())
            allowedMethods << ", ";
    }
    return allowedMethods.str();
}
