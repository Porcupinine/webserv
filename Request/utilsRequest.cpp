/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 15:50:31 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/05 14:49:24 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parseRequest.hpp"

std::string& parseRequest::capsOn(std::string &str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (str[0])
            str[0] = toupper(str[0]);
        if (str[i - 1] == '-' && isalpha(str[i]))
            str[i] = toupper(str[i]);
    }
    return str;
}

std::string parseRequest::rmSpaces(std::string &str) {
    size_t len;
    len = str.size() - 1;
    while (len >= 0 && str[len] == ' ')
        --len;
    return str.substr(0, len + 1);
}

std::vector<std::string> parseRequest::split(const std::string &str, char c) {
    std::vector<std::string> vec;
    std::string element;
    std::istringstream stream(str);

    while (std::getline(stream, element, c))
        vec.push_back(element);
    return vec;
}

bool cgiInvolved(const std::string& path) {
    std::size_t found = path.find("cgi-bin");
    // std::size_t foundExtPy = path.find(".py");
    // std::size_t foundExtSh = path.find(".sh");
    // std::size_t foundExtPl = path.find(".pl");
    
    // if (found != std::string::npos && (foundExtPy != std::string::npos
    // || foundExtPl != std::string::npos && foundExtSh != std::string::npos))
    if (found != std::string::npos)
        return true;
    return false;
}

