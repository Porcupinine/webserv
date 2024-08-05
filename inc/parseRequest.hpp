#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <list>
#include <utility>
#include <functional>

#include "response.h"
#include "defines.hpp"

class Response;
struct SharedData;

class parseRequest
{
    private:
        std::string _methodType;
        std::string _path;
        std::string _version;
        std::string _bodyMsg; 
        unsigned int _port;
        unsigned int _returnValue;
        std::string _query;
        std::map<std::string, std::string> _headers;
		std::list<std::pair<std::string, float>> _language;
        std::string _cgiresponse;
        std::map<std::string, std::string> _cookies;
        std::string _absPathRoot;


        void parseStr(std::string &info, struct SharedData* shared);

        void initHeaders();

        std::string readLine(const std::string &str, size_t &i);

        int parseFirstline(const std::string &info, struct SharedData* shared);
        int parsePath(const std::string &line, size_t i, struct SharedData &shared);
        int parseVersion(const std::string &line, size_t i);
        int validateMethodType();

        std::string setKey(const std::string &line);
        std::string setValue(const std::string &line);

        void setLanguage();
        void setQuery();
        std::string readBody(const std::string &str, size_t &i);

        std::map<std::string, std::string> parseCookies(const std::string& cookies);

        std::string& capsOn(std::string &str);
        std::string rmSpaces(std::string &str);
        std::vector<std::string> split(const std::string &str, char c);

    public:
		parseRequest(); // take the address of the struct 
        parseRequest(struct SharedData* shared); // ++ server instance -- TO DO
        ~parseRequest();
        parseRequest& operator=(const parseRequest &cpy);

        /* GETTERS */
        std::string getMethod(void) const;
        std::string getPath(void) const;
        std::string getVersion(void) const;
        unsigned int getPort(void) const;
        int getRetVal(void) const;
        std::string getBodyMsg(void) const;
        const std::map<std::string, std::string>& getHeaders(void) const;
        std::string getLanguageStr(void) const;
        std::string getQuery(void) const;
        std::string getCgiResponse(void) const;
        const std::map<std::string, std::string>& getCookies(void) const;

        /* SETTERS */
        void setMethod(std::string type);
        void setPath(std::string path);
        void setVersion(std::string v);
        void setPort(std::string port);
        void setRetVal(int value);
        void setBodyMsg(std::string b);

};




enum class Method
{
    GET,
    POST,
    DELETE
};


std::string     initMethodString(Method method);
bool            cgiInvolved(std::string path);



#endif