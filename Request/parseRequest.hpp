#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map> // needed??
#include <stack>
#include <vector>
#include <list>
#include <utility>
//#include <pair>
#include <functional>

class parseRequest
{
    private:
        std::string _methodType;
        std::string _path;
        std::string _version; // string or float?
        std::string _bodyMsg; 
        unsigned int _port; // init to 80 by default or what??
        unsigned int _returnValue; // to what do we init this?? 200
        std::string _query;
        std::string _infoStr; // that we got from the server's buffer
        std::map<std::string, std::string> _headers; // MAKE IT AN UNORERED MAP? NO why a map?
		std::list<std::pair<std::string, float>> _language;


        void parseStr(std::string &info);
        //bool validateStr();

        void initHeaders();

        std::string readLine(const std::string &str, size_t &i);

        int parseFirstline(const std::string &info);
        int parsePath(const std::string &line, size_t i);
        int parseVersion(const std::string &line, size_t i);
        int validateMethodType();

        std::string setKey(const std::string &line);
        std::string setValue(const std::string &line);

        std::string& capsOn(std::string &str); // maybe put this in utils
        std::string rmSpaces(std::string &str); // UTILS??
        std::vector<std::string> split(const std::string &str, char c); // UTILS??
        bool cgiInvolved(std::string path);

        void setLanguage();
        void setQuery();
        void setBody(const std::string &str);
        std::string readBody(const std::string &str, size_t &i);

    public:
        parseRequest(std::string &info);
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

std::string initMethodString(Method method);

#endif