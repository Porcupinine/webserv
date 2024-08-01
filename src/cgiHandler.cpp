//
// Created by laura on 21-4-24.
//

#include "parseRequest.hpp"
#include "cgiHandler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <cctype>
#include "defines.hpp"

// #define BUFFER_SIZE 100

//TODO need the server info
char **getEnv(parseRequest& request) {
	auto copy = request.getHeaders();
	char **env = new char*[copy.size() + 4];
	size_t count = 0;
	for (const auto & copy : copy) {
		std::string reform = copy.first;
		std::transform(reform.begin(), reform.end(), reform.begin(), ::toupper);
		std::replace( reform.begin(), reform.end(), '-', '_');
		std::string temp = reform + '=' + copy.second;
		env[count] = new char[copy.first.size()+copy.second.size()+2];
		size_t pos = 0;
		for (char & x : temp) {
			env[count][pos] = x;
			pos++;
		}
		env[count][pos] = '\0';
		count++;
	}
	std::string tmp = "REQUEST_METHOD=" + request.getMethod();
	env[count] = new char[tmp.size()];
	std::strcpy(env[count], tmp.data());
	count++;
	tmp = "QUERRY_STRING=";//TODO make a getter for querry
	env[count] = new char[tmp.size()];
	std::strcpy(env[count], tmp.data());
	count++;
	tmp = "UPLOAD_DIR=/sam/Codam/webserv/cgi-bin/uploads";//TODO make a get dir
	env[count] = new char[tmp.size()];
	std::strcpy(env[count], tmp.data());
	count++;
	tmp = "SERVER=";//TODO get SERVER from struct
	env[count] = new char[tmp.size()];
	std::strcpy(env[count], tmp.data());
	count++;
	env[count] = nullptr;
	return env;
}

void freeEnv(char **env) {
	for(int x = 0; env[x] != nullptr; x++) {
		delete env[x];
	}
	delete[] env;
}

int runChild(parseRequest& request, int pipeRead, int pipeWrite) {
	char *argv[] = {"./../cgi-bin/test.py", nullptr}; //path and NULL
	char **env = getEnv(request);
	int x = 0;
	std::cout<<"------env----\n";
	while(env[x] != nullptr){
		std::cout<<env[x]<<"\n";
		x++;
	}
	std::cout<<"------endv----\n";

	std::cout<<"FROM INSIDE the kids\n"<<"body: "<<request.getBodyMsg()<<"\n";
	if (dup2(pipeRead, STDIN_FILENO) == -1 || dup2(pipeWrite, STDOUT_FILENO) == -1) {
		std::cerr<<"Leave the kids alone!\n";
		close(pipeRead); // Close unused read end
		close(pipeWrite); // Close the original pipe write end
		freeEnv(env);
		return 1;
	}
	if (execve(argv[0], argv, env) == -1) {
		std::cerr<<"This is no middle age\n";
		close(pipeRead); // Close unused read end
		close(pipeWrite); // Close the original pipe write end
		freeEnv(env);
		return 1;
	}
	return 0;
}

int cgiHandler(SharedData* shared, parseRequest& request) {
	(void) shared;
	if (std::filesystem::exists(request.getPath())) {
		std::cerr<<"Sorry, can't find this file! Stop wasting my time!\n";
		return 1;
	}
	int pipeParentToChild[2]; // 0 - child parent read, 1 - parent write
	int pipeChildToParent[2]; // 0 - parent read, 1 - child write

	if (pipe(pipeParentToChild) == -1) {
		std::cerr<<"Pipe failed, you gotta call Mario!\n";
		return 1;
	}
	if (pipe(pipeChildToParent) == -1) {
		std::cerr<<"Pipe child failed, you gotta call Mario!\n";
		return 1;
	}
	int pid = fork();
	if (pid == -1) {
		std::cerr<<"There are no forks, you can try the philos!\n";
		return 1;
	}
	if (pid == 0) {
		close(pipeParentToChild[1]);
		close(pipeChildToParent[0]);
		runChild(request, pipeParentToChild[0], pipeChildToParent[1]);
	} else {
		close(pipeParentToChild[0]);
		close(pipeChildToParent[1]);
		auto body = request.getBodyMsg();
		write(pipeParentToChild[1], body.data(), body.size());
		close(pipeParentToChild[1]);
		std::cout<<"body done!\n";

		int buffLen = 0;
		std::string response;
		std::string buffer(BUFFER_SIZE, '\0');
		while ((buffLen = ::read(pipeChildToParent[0], buffer.data(), BUFFER_SIZE)) > 0) {
//			std::cout << "Parent: " << buffer.substr(0, buffLen) << "\nEND\n";
			response.append(buffer.data(), buffer.length());
		}
		if (buffLen < 0) {
			std::cerr<<"Failed to read!\n";
		}
		std::cout<<"------------response-------------\n"<<response<<"\n";
		close(pipeParentToChild[0]); // Close the read end after reading
		wait(nullptr); // Wait for child process to finish
	}
	return 0;
}

//TODO, adjust return to string ???

//TODO, should I throw errors or use old approach

//TODO make index page with cgi to show cookie after user fills the name and intra login?

//TODO open pipes on Luś end
/* getting a struct with server info and epolFD
 * get the parameters needed for the env and add the pipe fds to the epoll*/
