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
#include "defines.hpp"
#include "parseRequest.hpp"

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

        /*RESPONSE HEADER FIELDS*/
        std::string					_allow;
        std::string					_contentLanguage;
        // std::string					_contentLength;
        long unsigned int           _contentLength;
        std::string					_contentType;
        std::string					_date;
        std::string					_location;
        std::vector<std::string>    _setcookies;

    public:
        Response(void);
        ~Response();
        Response& operator=(const Response &cpy);

        std::string giveResponse(parseRequest& request, struct SharedData &shared);

        void getMethod(parseRequest& request, struct SharedData* shared);
        void postMethod(parseRequest& request, struct SharedData* shared);
        void deleteMethod(parseRequest& request, struct SharedData* shared);

        void initErrorCodes();
        void htmlErrorCodesMap();

        /* COOKIE RELATED */
        std::vector<std::string> createSetCookie(const std::map<std::string, std::string> &cookieMap);
        std::string getFormattedTime(time_t rawtime);

        /* RESPONSE HEADER BUIDLING RELATED */
        std::string buildResponseHeader(parseRequest& request, struct SharedData* shared);
        void initResponseHeaderFields();
        void setHeaderValues(parseRequest& request);
        std::string setAllow();
        std::string setDate();
        std::string getHeaderValues(parseRequest& request, std::string header, struct SharedData* shared);
        std::string getMatchingCodeString(unsigned int code);

        /* HTML RELATED */
        std::string errorHtml(unsigned int error, struct SharedData* shared, parseRequest &request);
        void readContent(parseRequest& request, struct SharedData* shared);

        /* GETTERS */
        std::string getResponse(void) const; 
        std::string getVersion(void) const;
        unsigned int getStatusCode(void) const;

        /* UTILS */
        bool fileExists(const std::string& path);

        /* STATIC */
		using ResponseCallback = void (Response::*)(parseRequest&, struct SharedData* shared);
        static std::map<std::string, ResponseCallback>	_method;
	    static std::map<std::string, ResponseCallback>	initMethods();

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
