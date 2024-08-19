/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:45:44 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:46:55 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "../inc/defines.hpp"
#include "../inc/ParseRequest.hpp"

class ParseRequest;

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

        std::string giveResponse(ParseRequest& request, struct SharedData &shared);

        void getMethod(ParseRequest& request, struct SharedData* shared);
        void postMethod(ParseRequest& request, struct SharedData* shared);
        void deleteMethod(ParseRequest& request, struct SharedData* shared);

        void initErrorCodes();
        void htmlErrorCodesMap();

        /* COOKIE RELATED */
        std::vector<std::string> createSetCookie(const std::map<std::string, std::string> &cookieMap);
        std::string getFormattedTime(time_t rawtime);

        /* RESPONSE HEADER BUIDLING RELATED */
        std::string buildResponseHeader(ParseRequest& request, struct SharedData* shared);
        void initResponseHeaderFields();
        void setHeaderValues(ParseRequest& request);
        std::string setAllow();
        std::string setDate();
        std::string getHeaderValues(ParseRequest& request, std::string header, struct SharedData* shared);
        std::string getMatchingCodeString(unsigned int code);

        /* HTML RELATED */
        std::string errorHtml(unsigned int error, struct SharedData* shared, ParseRequest &request);
        void readContent(ParseRequest& request, struct SharedData* shared);

        /* GETTERS */
        std::string getResponse(void) const; 
        std::string getVersion(void) const;
        unsigned int getStatusCode(void) const;

        /* UTILS */
        bool fileExists(const std::string& path);

        /* STATIC */
		using ResponseCallback = void (Response::*)(ParseRequest&, struct SharedData* shared);
        static std::map<std::string, ResponseCallback>	_method;
	    static std::map<std::string, ResponseCallback>	initMethods();

        /* UTILS FOR AUTO INDEX */
        std::string autoIndexPageListing(const std::string& path, const std::string& dir);
};


/* ELEMENTS OF HTTP REPONSE
    protocol version: HTTP/1.1
    status code: 200
    status text: OK
    headers
    body (not always)

*/


#endif //WEBSERV_RESPONSE_HPP
