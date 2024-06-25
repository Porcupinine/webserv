//
// Created by laura on 21-4-24.
//

#include "../Request/parseRequest.hpp"
#include "../includes/cgiHandler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <filesystem>

#define BUFFER_SIZE 100

char **convertEnv(const std::map<std::string, std::string> &mapHeaders) {
	auto copy = mapHeaders;
//	copy["REQUEST_METHOD"] = "POST";
	char **env = new char*[copy.size()];
	size_t count = 0;
	for (const auto & mapHeader : copy) {
		std::string temp = mapHeader.first + '=' + mapHeader.second;
		std::cout<<"env: "<<temp<<"\n";
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

char **getEnv(parseRequest& request) {
	char **env = new char*[6];
	std::string tmp = "REQUEST_METHOD=" + request.getMethod();
	std::cout<<"env: "<<tmp<<"\n";
	env[0] = new char[tmp.size()];
	std::strcpy(env[0], tmp.data());
	tmp = "QUERRY_STRING=";//TODO make a getter for querry
	std::cout<<"env: "<<tmp<<"\n";
	env[1] = new char[tmp.size()];
	std::strcpy(env[1], tmp.data());
	tmp = "REMOTE_HOST=/upload.py";//TODO make a get host
	std::cout<<"env: "<<tmp<<"\n";
	env[2] = new char[tmp.size()];
	std::strcpy(env[2], tmp.data());
	tmp = "PATH_INFO=";//TODO make get path
	std::cout<<"env: "<<tmp<<"\n";
	env[3] = new char[tmp.size()];
	std::strcpy(env[3], tmp.data());
	tmp = "UPLOAD_DIR=";//TODO make a get dir
	std::cout<<"env: "<<tmp<<"\n";
	env[4] = new char[tmp.size()];
	std::strcpy(env[4], tmp.data());
	env[5] = nullptr;
	return env;
}

void freeEnv(char **env) {
	for(int x = 0; env[x] != nullptr; x++) {
		delete env[x];
	}
	delete[] env;
}
//TODO check if the file exist
int runChild(parseRequest& request, int pipeRead, int pipeWrite) {
//	size_t body_len = request.getBodyMsg().length() + 1;
//	char *body = new char[body_len];
//	std::strcpy(body, request.getBodyMsg().data()); //TODO, can I use this?

	char *argv[] = {"./../cgi-bin/test.py", nullptr}; //path and NULL
	char **env = convertEnv(request.getHeaders());

	std::cout<<"FROM INSIDE the kids\n"<<"body: "<<request.getBodyMsg()<<"\n";
	if (dup2(pipeRead, STDIN_FILENO) == -1 || dup2(pipeWrite, STDOUT_FILENO) == -1) {
		std::cerr<<"Leave the kids alone!\n";
//		delete [] body;
		close(pipeRead); // Close unused read end
		close(pipeWrite); // Close the original pipe write end
		return 1;
	}
//	int buffLen = 0;
//	char bufferData[100];
//	std::string response;
//	while ((buffLen = ::read(pipeRead, bufferData, BUFFER_SIZE)) > 0) {
////		std::cout << "Parent: " << buffer << "\nEND\n";
//		response.append(bufferData, buffLen);
//	}

	if (execve(argv[0], argv, env) == -1) {
		std::cerr<<"This is no middle age\n";
		close(pipeRead); // Close unused read end
		close(pipeWrite); // Close the original pipe write end
		freeEnv(env);
		return 1;
	}
	return 0;
	//TODO  where to free the env??
}

int cgiHandler(parseRequest& request) {
	if (std::filesystem::exists(request.getPath())) {
		std::cout<<"Sorry, can't find this file\n";
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
		runChild(request, pipeParentToChild[0], pipeChildToParent[1]);//we got a kid, lets run shit here
		//TODO Do we come back here?
	} else {
		close(pipeParentToChild[0]);
		close(pipeChildToParent[1]);
		auto body = request.getBodyMsg();
		write(pipeParentToChild[1], body.data(), body.size());
		close(pipeParentToChild[1]);
		int buffLen = 0;
		std::string response;
		std::string buffer(BUFFER_SIZE, '\0');
		while ((buffLen = ::read(pipeChildToParent[0], buffer.data(), BUFFER_SIZE)) > 0) {
			std::cout << "Parent: " << buffer << "\nEND\n";
			response.append(buffer.data(), buffer.length());
		}
		if (buffLen < 0) {
			std::cerr<<"Failed to read!\n";
		} //TODO why it will always be 0 ?
		std::cout<<"------------response-------------\n"<<response<<"\n";
		close(pipeParentToChild[0]); // Close the read end after reading
		wait(nullptr); // Wait for child process to finish
	}
	return 0;
}
//TODO, should I throw errors or use old approach
