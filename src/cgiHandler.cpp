//
// Created by laura on 21-4-24.
//

#include "../Request/parseRequest.hpp"
#include "../includes/cgiHandler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#define BUFFER_SIZE 100

char **convertEnv(const std::map<std::string, std::string> &mapHeaders) {
	char **env = new char*[mapHeaders.size()];
	size_t count = 0;
	for (const auto & mapHeader : mapHeaders) {
		std::string temp = mapHeader.first + '=' + mapHeader.second;
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

void freeEnv(char **env) {
	for(int x = 0; env[x] != nullptr; x++) {
		delete env[x];
	}
	delete[] env;
}

int runRequest(parseRequest& request, int *pipeFd) {
	char *argv[] = {"./../cgi-bin/test.py", nullptr}; //path and NULL
	char **env = convertEnv(request.getHeaders());

	std::cout<<"call the kids\n";

	if (dup2(pipeFd[0], STDIN_FILENO) == -1 || dup2(pipeFd[1], STDOUT_FILENO) == -1) {
		std::cerr<<"Leave the kids alone!\n";
		close(pipeFd[0]); // Close unused read end
		close(pipeFd[1]); // Close the original pipe write end
		return 1;
	}
	close(pipeFd[0]); // Close unused read end
	close(pipeFd[1]); // Close the original pipe write end

	if (execve(argv[0], argv, env) == -1) {
		std::cerr<<"This is no middle age\n";
	}
	return 0;
	//TODO  where to free the enviroment
}

int cgiHandler(parseRequest& request) {
	int pipeFd[2];

	if (pipe(pipeFd) == -1) {
		std::cerr<<"Pipe failed, you gotta call Mario!\n";
		return 1;
	}
	int pid = fork();
	if (pid == -1) {
		std::cerr<<"There are no forks, you can try the philos!\n";
		return 1;
	}
	if (pid == 0) {
		runRequest(request, pipeFd);//we got a kid, lets run shit here
	} else {
		close(pipeFd[1]); // Close unused write end

		size_t buffLen = 0;
		std::string response;
		std::string buffer(BUFFER_SIZE, '\0');
		while ((buffLen = ::read(pipeFd[0], buffer.data(), BUFFER_SIZE)) > 0) {
			response.append(buffer.data(), buffer.length());
		}
		if (buffLen < 0) {
			std::cerr<<"Failed to read!\n";
		}
		std::cout<<"response: "<<response<<"\n";
		close(pipeFd[0]); // Close the read end after reading
		wait(nullptr); // Wait for child process to finish
	}
	return 0;
}
