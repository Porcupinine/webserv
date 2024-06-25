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
        std::string     _version; // (like HTTP/1.1)
        unsigned int    _statusCode; // (like 200) or could we use _returnValue from parseRequest??
        // std::string     _statusText; // (like OK)
        //std::string _respHeaders; // needed?? or make a map out of it?? PUT THIS ESLEWHERE PERHAPS? IN ANOTHER CLASS
        //std::string     _respBody; // using ??
        // std::string     _setCookie; // do we want/need this??
        std::string     _type;
        int             _port;
        std::string     _host;
        bool            _isAutoIndex; // meaning autoIndex = true; means to respond with /path/index.html when /path/ is requested
        std::map<unsigned int, std::string> _errorCodes;
        std::map<unsigned int, std::string> _errorCodesHtml;

        /*RESPONSE HEADER FIELDS*/
        std::string					_allow;
        std::string					_contentLanguage;
        std::string					_contentLength;
        std::string					_contentLocation;
        std::string					_contentType;
        std::string					_date;
        std::string					_lastModified;
        std::string					_location;
        std::string					_retryAfter;
        std::string					_server;
        std::string					_transferEncoding;

    public:
        Response(void);
        ~Response();
        Response& operator=(const Response &cpy);

        void giveResponse(parseRequest& request);

        // add everything else also here in public, functions related to methods blabla
        void getMethod(parseRequest& request); // arg needed??
        void postMethod(parseRequest& request); // arg needed?
        void deleteMethod(parseRequest& request);

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
        // std::string readHtmlFile(const std::string &path);
        void readContent(parseRequest& request); // check if the above can be merged with this one

        /* GETTERS */
        std::string getResponse(void) const; 
        std::string getVersion(void) const;
        unsigned int getStatusCode(void) const;
        std::string getStatusText(void) const;
        std::string getRespBody(void) const;

        /* UTILS */
        bool fileExists(const std::string& path);

        /* STATIC */
        // using MethodHandler = std::function<void(Response*, parseRequest&, const std::string&)>;
        // std::map<std::string, MethodHandler> _method;
        static std::map<std::string, void (Response::*)(parseRequest&)>	_method;
	    static std::map<std::string, void (Response::*)(parseRequest&)>	initMethods();

        /* UTILS FOR AUTO INDEX */
        std::string autoIndexPageListing(const std::string& path, const std::string& host, int port);
};


/* ELEMENTS OF HTTP REPONSE
    protocol version: HTTP/1.1
    status code: 200
    status text: OK
    headers
    body (not always)

*/


#endif //WEBSERV_RESPONSE_H
