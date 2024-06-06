#include "../Request/parseRequest.hpp"

std::string& parseRequest::capsOn(std::string &str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (str[0])
            str[0] = toupper(str[0]);
        if (str[i - 1] == '-' && isalpha(str[i])) // this correct or !=
            str[i] = toupper(str[i]);
    }
    return str;
}

std::string parseRequest::rmSpaces(std::string &str) {
    size_t len;
    len = str.size() - 1;
    while (len >= 0 && str[len] == ' ')
        --len;
    return str.substr(0, len + 1); // to test
}

std::vector<std::string> parseRequest::split(const std::string &str, char c) {
    std::vector<std::string> vec;
    std::string element;
    std::istringstream stream(str);

    while (std::getline(stream, element, c))
        vec.push_back(element);
    return vec;
}

bool parseRequest::cgiInvolved(std::string path) {
    std::size_t found = path.find("cgi-bin");
    
    if (found != std::string::npos)
        return true;
    return false;
}
