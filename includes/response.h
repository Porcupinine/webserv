//
// Created by laura on 16-5-24.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include <iostream>
#include <sys/stat.h>
#include <fstream>

#include "../Request/parseRequest.hpp"

class parseRequest;

class Response 
{
    private:
	    std::string     _response{}; // final response string
        std::string     _version; // (like HTTP/1.1)
        unsigned int    _statusCode; // (like 200) or could we use _returnValue from parseRequest??
        std::string     _statusText; // (like OK)
        //std::string _respHeaders; // needed?? or make a map out of it?? PUT THIS ESLEWHERE PERHAPS? IN ANOTHER CLASS
        std::string     _respBody; // using ??
        // std::string     _setCookie; // do we want/need this??
        std::string     _type;
        bool            _autoIndex; // meaning autoIndex = true; means to respond with /path/index.html when /path/ is requested
        std::map<unsigned int, std::string> _errorCodes;


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

        std::string buildResponseHeader(); // check if args needed

        /* HTML RELATED */
        std::string errorHtml(unsigned int error);
        std::string readHtmlFile(const std::string &path);
        int readContent(void); // check if the above can be merged with this one

        /* GETTERS */
        std::string getResponse(void) const; 
        std::string getVersion(void) const;
        unsigned int getStatusCode(void) const;
        std::string getStatusText(void) const;
        std::string getRespBody(void) const;

        /* UTILS */
        bool fileExists(const std::string& path);

        /* STATIC */
        static std::map<std::string, void (Response::*)(parseRequest&)>	_method;
	    static std::map<std::string, void (Response::*)(parseRequest&)>	initMethods();

};


/* ELEMENTS OF HTTP REPONSE
    protocol version: HTTP/1.1
    status code: 200
    status text: OK
    headers
    body (not always)

*/


#endif //WEBSERV_RESPONSE_H
