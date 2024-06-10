//
// Created by laura on 21-4-24.
//

#include "../Request/parseRequest.hpp"
#include "../includes/cgiHandler.h"
#include <unistd.h>
#include <string>
#define BUFFER_SIZE 100

char **convertEnv(const std::map<std::string, std::string> &mapHeaders) {
	char **env = new char*[mapHeaders.size()];
	size_t count = 0;
	for (const auto & mapHeader : mapHeaders) {
		std::string temp = mapHeader.first + '=' + mapHeader.second;
		std::cout<<"str: "<<temp<<"\n";
		env[count] = new char[mapHeader.first.size()+mapHeader.second.size()+2];
		size_t pos = 0;
		for (char & x : temp) {
			env[count][pos] = x;
			pos++;
		}
		env[count][pos] = '\0';
		count++;
	}
	env[count] = nullptr;
	return env;
}

int runRequest(parseRequest& request, int *pipeFd) {
	//get the right path to run
	//seta as variaveis no filho
	char *argv[2]; //path and NULL
	char **env = convertEnv(request.getHeaders()); //need to convert the map into a char** after all
	static std::string test = "../cgi-bin/test.py";
	argv[0] = test.data();
	argv[1] = nullptr;
	for (size_t it = 0; env[it] != nullptr; it++) {
		std::cout<<"char**: "<<env[it]<<"\n";
	}

	if (dup2(pipeFd[0], STDIN_FILENO) == -1 || dup2(pipeFd[1], STDOUT_FILENO) == -1) {
		std::cout<<"Leave the kids alone!\n";
		//TODO do I need to close the fds?
		return 1;
	}
	std::string line;
	std::string buffer;
	size_t buff_len = 0;
	std::cout<<"call the kids\n";
	execv(reinterpret_cast<const char *>(argv), env);
	while ((buff_len = ::read(pipeFd[1], buffer.data(), BUFFER_SIZE)) > 0) {
		line.append(buffer.data(), buff_len);
	}
	std::cout<<line<<"\n";
	for(int x = 0; env[x] != nullptr; x++) {
		delete env[x];
	}
	delete[] env;
	return 0;
}

int cgiHandler(parseRequest& request) {
	int pipeFd[2];

	if (pipe(pipeFd) == -1) {
		std::cout<<"Pipe failed, you gotta call Mario!\n";
		return 1;
	}
	int pid = fork();
	if (pid == -1) {
		std::cout<<"There are no forks, you can try the philos!\n";
		return 1;
	}
	if (pid == 0) {
		runRequest(request, pipeFd);//we got a kid, lets run shit here
	}
	return 0;
}
