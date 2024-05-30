//
// Created by laura on 21-4-24.
//

#include "cgiHandler.h"
#include "../Request/parseRequest.hpp"
#include <unistd.h>
#include <string>

int runRequest(parseRequest& request, int *pipeFd) {
	//get the right path to run
	//seta as variaveis no filho
	const char **argv = getPath(request); //path and NULL
	char **env;

	if (dup2(pipeFd[0], STDIN_FILENO) == -1 || dup2(pipeFd[1], STDOUT_FILENO) == -1) {
		std::cout<<"Leave the kids alone!\n";
		//TODO do I need to close the fds?
		return 1;
	}
	execv(argv, env)
}

int cgiHandler(parseRequest& request) {
	int pipeFd[2];

	if (pipe(pipeFd) == -1) {
		std::cout<<"Pipe failed, you gotta call Mario!\n";
		return 1
	}
	pid_t pid = fork();
	if (pid == -1) {
		std::cout<<"There are no forks, you can try the philos!\n";
		return 1;
	}
	if (pid == 0) {
		//we got a kid, lets run shit here
	}
	return 0;
}
