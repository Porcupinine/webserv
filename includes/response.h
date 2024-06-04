//
// Created by laura on 16-5-24.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include  <iostream>
#include <sys/stat.h>

#include "../Request/parseRequest.hpp"



class Response 
{
    private:
	    std::string	_response{}; // final response string
        std::string _version; // (like HTTP/1.1)
        unsigned int _statusCode; // (like 200) or could we use _returnValue from parseRequest??
        std::string _statusText; // (like OK)
        //std::string _respHeaders; // needed?? or make a map out of it?? PUT THIS ESLEWHERE PERHAPS? IN ANOTHER CLASS
        std::string _respBody;

    public:
        Response(void);
        ~Response();
        Response& operator=(const Response &cpy);

        // add everything else also here in public, functions related to methods blabla
        void getMethod(); // arg needed??
        void postMethod(); // arg needed?
        void deleteMethod(parseRequest& request);

        /* GETTERS */
        std::string getResponse(void) const; 
        std::string getVersion(void) const;
        unsigned int getStatusCode(void) const;
        std::string getStatusText(void) const;
        std::string getRespBody(void) const;

        /* UTILS */
        bool fileExists(std::string path);

};


/* ELEMENTS OF HTTP REPONSE
    protocol version: HTTP/1.1
    status code: 200
    status text: OK
    headers
    body (not always)

*/


#endif //WEBSERV_RESPONSE_H
