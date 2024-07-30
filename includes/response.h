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
#include "../inc/defines.hpp"

#include "../Request/parseRequest.hpp"

class parseRequest;

class Response 
{
    private:
	    std::string     _response{};
        std::string     _version;
        unsigned int    _statusCode;
        std::string     _type;
        bool            _isAutoIndex;
        std::map<unsigned int, std::string> _errorCodes;
        std::map<unsigned int, std::string> _errorCodesHtml;
        std::string      _absrootpath;

        /*RESPONSE HEADER FIELDS*/
        std::string					_allow;
        std::string					_contentLanguage;
        std::string					_contentLength;
        std::string					_contentType;
        std::string					_date;
        std::string					_location;
        std::vector<std::string>    _setcookies;

    public:
        Response(void);
        ~Response();
        Response& operator=(const Response &cpy);

        std::string giveResponse(parseRequest& request, SharedData* shared);

        void getMethod(parseRequest& request, SharedData* shared);
        void postMethod(parseRequest& request, SharedData* shared);
        void deleteMethod(parseRequest& request, SharedData* shared);

        void initErrorCodes();
        void htmlErrorCodesMap();

        /* COOKIE RELATED */
        std::vector<std::string> createSetCookie(const std::map<std::string, std::string> &cookieMap);
        std::string getFormattedTime(time_t rawtime);

        /* RESPONSE HEADER BUIDLING RELATED */
        std::string buildResponseHeader(parseRequest& request, SharedData* shared); // check if args needed
        void initResponseHeaderFields();
        void setHeaderValues(parseRequest& request);
        std::string setAllow(parseRequest& request);
        std::string setDate(parseRequest& request);
        std::string getHeaderValues(parseRequest& request, std::string header, SharedData* shared);
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
