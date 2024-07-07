//
// Created by laura on 16-5-24.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "../Request/parseRequest.hpp"

class parseRequest;

class Response 
{
    private:
	    std::string     _response{}; // response body string, storing the HTML
        std::string     _version;
        unsigned int    _statusCode;
        // std::string     _setCookie; // do we want/need this??
        std::string     _type;
        bool            _isAutoIndex; // meaning autoIndex = true; means to respond with /path/index.html when /path/ is requested
        std::map<unsigned int, std::string> _errorCodes;
        std::map<unsigned int, std::string> _errorCodesHtml;

        /*RESPONSE HEADER FIELDS*/
        std::string					_allow;
        std::string					_contentLanguage;
        std::string					_contentLength;
        std::string					_contentType;
        std::string					_date;
        std::string					_location;

    public:
        Response(void);
        ~Response();
        Response& operator=(const Response &cpy);

        std::string giveResponse(parseRequest& request);

        void getMethod(parseRequest& request); // arg needed?? yes from config file
        void postMethod(parseRequest& request); // arg needed?? yes from config file
        void deleteMethod(parseRequest& request); // arg needed?? yes from config file

        void initErrorCodes();
        void htmlErrorCodesMap();

        /* RESPONSE HEADER BUIDLING RELATED */
        std::string buildResponseHeader(parseRequest& request); // check if args needed
        void initResponseHeaderFields();
        void setHeaderValues(parseRequest& request);
        std::string setAllow(parseRequest& request);
        std::string setDate(parseRequest& request);
        std::string getHeaderValues(parseRequest& request, std::string header);
        std::string getMatchingCodeString(unsigned int code);

        /* HTML RELATED */
        std::string errorHtml(unsigned int error);
        void readContent(parseRequest& request);

        /* GETTERS */
        std::string getResponse(void) const; 
        std::string getVersion(void) const;
        unsigned int getStatusCode(void) const;

        /* UTILS */
        bool fileExists(const std::string& path);

        /* STATIC */
        static std::map<std::string, void (Response::*)(parseRequest&)>	_method;
	    static std::map<std::string, void (Response::*)(parseRequest&)>	initMethods();

        /* UTILS FOR AUTO INDEX */
        std::string autoIndexPageListing(const std::string& path);
};


/* ELEMENTS OF HTTP REPONSE
    protocol version: HTTP/1.1
    status code: 200
    status text: OK
    headers
    body (not always)

*/


#endif //WEBSERV_RESPONSE_H
